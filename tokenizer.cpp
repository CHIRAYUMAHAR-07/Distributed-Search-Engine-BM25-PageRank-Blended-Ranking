#include "tokenizer.h"
#include <sstream>
#include <algorithm>
#include <cctype>

Tokenizer::Tokenizer()
{
    stopwords = {
        "the", "is", "at", "which", "on", "and", "a", "an", "of", "to", "in", "for"};
}

std::string Tokenizer::normalize(const std::string &word)
{
    std::string w = word;
    std::transform(w.begin(), w.end(), w.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    w.erase(std::remove_if(w.begin(), w.end(),
                           [](char c)
                           { return !std::isalnum(c); }),
            w.end());
    return w;
}

std::vector<std::string> Tokenizer::tokenize(const std::string &text)
{
    std::stringstream ss(text);
    std::string word;
    std::vector<std::string> tokens;

    while (ss >> word)
    {
        std::string norm = normalize(word);
        if (!norm.empty() && stopwords.find(norm) == stopwords.end())
        {
            tokens.push_back(norm);
        }
    }
    return tokens;
}
