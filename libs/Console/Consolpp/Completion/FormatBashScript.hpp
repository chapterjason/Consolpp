#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Completion
{
    // The script asks the program with "_complete <current> -- <words>" and keeps to Bash 3.2, the Bash of macOS.
    // Bash splits words at the characters of COMP_WORDBREAKS, so the script joins the parts of a word again, and each
    // suggestion loses what Bash does not replace: the typed word up to its last ':' or '='. The words reach the
    // program without their quoting, and the suggestions are quoted again; Bash 3.2 has no compopt, so there paths
    // are quoted by the script and directories get no '/'.
    template <std::output_iterator<char> Output>
    constexpr Output formatBashScript(const std::string_view programName, Output output)
    {
        constexpr std::string_view placeholder{"%function%"};
        constexpr std::string_view script{R"bash(%function%_dequote()
{
    local word=$1 result= quote= character index

    for ((index = 0; index < ${#word}; ++index)); do
        character=${word:index:1}

        if [[ $quote == "'" ]]; then
            if [[ $character == "'" ]]; then
                quote=
            else
                result+=$character
            fi
        elif [[ $character == '\' && -z $quote ]]; then
            ((++index))
            result+=${word:index:1}
        elif [[ $character == '\' ]]; then
            case ${word:index+1:1} in
                '$' | '`' | '"' | '\')
                    ((++index))
                    result+=${word:index:1}
                    ;;
                *)
                    result+=$character
                    ;;
            esac
        elif [[ -n $quote && $character == "$quote" ]]; then
            quote=
        elif [[ -z $quote && ( $character == "'" || $character == '"' ) ]]; then
            quote=$character
        else
            result+=$character
        fi
    done

    printf '%s' "$result"
}

%function%()
{
    local -a words=()
    local current=0 position=0 index word gap rest prefix kind valuePrefix option escape suggestion

    for ((index = 0; index < ${#COMP_WORDS[@]}; ++index)); do
        word=${COMP_WORDS[index]}
        gap=${COMP_LINE:position}
        gap=${gap%%"$word"*}
        position=$((position + ${#gap} + ${#word}))

        if ((index > 0)) && [[ -n $word && -z $gap ]]; then
            words[${#words[@]}-1]+=$word
        else
            words+=("$word")
        fi

        if ((index == COMP_CWORD)); then
            current=$((${#words[@]} - 1))
        fi
    done

    for ((index = 1; index < ${#words[@]}; ++index)); do
        words[index]=$(%function%_dequote "${words[index]}")
    done

    rest=${words[current]}

    if [[ $COMP_WORDBREAKS == *:* ]]; then
        rest=${rest##*:}
    fi

    if [[ $COMP_WORDBREAKS == *=* ]]; then
        rest=${rest##*=}
    fi

    prefix=${words[current]%"$rest"}
    COMPREPLY=()

    {
        IFS= read -r kind

        if [[ $kind == words ]]; then
            while IFS= read -r suggestion; do
                printf -v suggestion '%q' "${suggestion#"$prefix"}"
                COMPREPLY+=("$suggestion")
            done
        elif [[ -n $kind ]]; then
            IFS= read -r valuePrefix
            option=-f
            escape=1

            if [[ $kind == directories ]]; then
                option=-d
            fi

            if compopt -o filenames 2>/dev/null; then
                escape=
            fi

            while IFS= read -r suggestion; do
                suggestion=$valuePrefix$suggestion
                suggestion=${suggestion#"$prefix"}

                if [[ -n $escape ]]; then
                    printf -v suggestion '%q' "$suggestion"
                fi

                COMPREPLY+=("$suggestion")
            done < <(compgen "$option" -- "${words[current]#"$valuePrefix"}")
        fi
    } < <("${words[0]}" _complete "$((current - 1))" -- "${words[@]:1}" 2>/dev/null)
}

complete -F %function% )bash"};
        const auto writeFunctionName = [programName, &output] {
            output = std::ranges::copy(std::string_view{"_"}, output).out;

            for (const char character : programName) {
                const bool kept = (character >= 'a' && character <= 'z')
                                  || (character >= 'A' && character <= 'Z')
                                  || (character >= '0' && character <= '9');

                *output = kept ? character : '_';
                ++output;
            }

            output = std::ranges::copy(std::string_view{"_complete"}, output).out;
        };

        for (std::size_t start = 0; start < script.size();) {
            const std::size_t found = std::min(script.find(placeholder, start), script.size());

            output = std::ranges::copy(script.substr(start, found - start), output).out;

            if (found < script.size()) {
                writeFunctionName();
            }

            start = found + placeholder.size();
        }

        output = std::ranges::copy(std::string_view{"'"}, output).out;

        for (const char character : programName) {
            if (character == '\'') {
                output = std::ranges::copy(std::string_view{R"('\'')"}, output).out;

                continue;
            }

            *output = character;
            ++output;
        }

        return std::ranges::copy(std::string_view{"'\n"}, output).out;
    }
} // namespace Consolpp::Completion
