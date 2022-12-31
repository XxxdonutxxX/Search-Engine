#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void indexFile(std::ifstream &file,
                std::unordered_map<std::string, std::unordered_map<int, std::vector<int>>> &stopWords,
                std::vector<std::vector<std::string>> &questionsList,
                std::unordered_set<std::string> &allWords,
                std::vector<std::unordered_map<std::string, std::string>> &index,
                int &stoppedLine) {
    std::string question;
    std::string answer;
    std::string line;
    int currIndx = 0;

    while (std::getline(file, line)) {
        if (line == "\n") {
            currIndx++;
            continue;
        }
        if (line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n') {
            line = line.substr(0, line.length() - 1);
        }

        if (line.rfind("{}{}", 0) == 0) {
            line = line.substr(4, line.length() - 1);
        }

        for (int i = 0; i < line.length(); i++) {
            question += line[i];
            if (line[i] == '|' && line[i + 1] == '|' && line[i + 2] == '|' && line[i + 3] == '|') {
                question.pop_back();
                line = line.substr(i + 4, line.length() - 1);
                break;
            }
        }

        while (!ends_with(line, "{}{}")) {
            std::erase(line, '\n');
            while(!line.empty() && std::isspace(*line.begin()))
                line.erase(line.begin());

            while(!line.empty() && std::isspace(*line.rbegin()))
                line.erase(line.length()-1);

            answer += line + " ";
            std::getline(file, line);
        }

        std::erase(line, '\n');
        while(!line.empty() && std::isspace(*line.begin()))
            line.erase(line.begin());

        while(!line.empty() && std::isspace(*line.rbegin()))
            line.erase(line.length()-1);

        answer += line.substr(0, line.length() - 4);

        std::erase(question, '\n');
        std::erase(answer, '\n');
        while(!answer.empty() && std::isspace(*answer.begin()))
            answer.erase(answer.begin());

        while(!answer.empty() && std::isspace(*answer.rbegin()))
            answer.erase(answer.length()-1);

        while(!question.empty() && std::isspace(*question.begin()))
            question.erase(question.begin());

        while(!answer.empty() && std::isspace(*question.rbegin()))
            question.erase(question.length()-1);

        questionsList.emplace_back();
        index.emplace_back();
        index[currIndx] = {{question, answer}};

        std::string temp;
        std::stringstream ss(question);
        int wordPos = 0;

        while (std::getline(ss, temp, ' ')) {
            stopWords[temp][currIndx].push_back(wordPos);
            questionsList[currIndx].push_back(temp);
            allWords.insert(temp);
            wordPos++;
        }

        question = "";
        answer = "";
        currIndx++;
        }
    stoppedLine++;
    file.close();
}

unsigned int edit_distance(const std::string& s1, const std::string& s2) {
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

    d[0][0] = 0;
    for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
    for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

    for(unsigned int i = 1; i <= len1; ++i)
        for(unsigned int j = 1; j <= len2; ++j)
            d[i][j] = std::min(std::min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
    return d[len1][len2];
}

float getSimilarity(const std::string& s1, const std::string& s2) {
    return 1 - 1.0 * edit_distance(s1, s2) / std::max(s1.size(), s2.size());
}

int compareTwoStrings(std::vector<std::string> &words1, std::vector<std::string> &words2,
                        const std::unordered_map<std::string, std::vector<int>>& map,
                        const std::unordered_set<std::string>& allWords)
{
    double similarity = 0;
    std::vector<int> positions;
    std::string finalString = "";

    for (auto &word : words2) {
        auto temp = getSimilarity(word, words1[0]);
        if (temp > similarity) {
            similarity = temp;
            finalString = word;
        }
    }

    for (int i = 0; i < words2.size(); i++) {
        if (getSimilarity(words2[i], finalString) > 0.8) {
            positions.push_back(i);
        }
    }

    for (int position : positions) {
        for (int j = 0; j < words1.size() && position + j < words2.size(); j++) {
            similarity += getSimilarity(words1[j], words2[position + j]);
        }
    }

    return int((similarity / std::min(words1.size(), words2.size())) * 100);
}

int main( int argc, char *argv[] ) {
    std::unordered_map<std::string, std::unordered_map<int, std::vector<int>>> stopWords;
    std::vector<std::vector<std::string>> questionsList;
    std::unordered_set<std::string> allWords;
    std::ifstream file(argv[1]);
    std::vector<std::unordered_map<std::string, std::string>> map;
    int stoppedLine = 0;

    indexFile(file,stopWords, questionsList, allWords, map, stoppedLine);

    //while (1) {
        std::string question = argv[2];
        std::string bestAnswer;
        double bestSim = 0;

        //std::cout << "[][]?[][]: ";
        //std::getline(std::cin, question);

        std::erase(question, '\n');
        std::erase(question, '\r');

        while(!question.empty() && std::isspace(*question.begin())) {
            question.erase(question.begin());
        }
        while(!question.empty() && std::isspace(*question.rbegin())) {
            question.erase(question.length() - 1);
        }

        // check if question has the string "||||" in it
        if (question.find("||||") != std::string::npos) {
            std::string question1 = question.substr(0, question.find("||||"));
            std::string question2 = question.substr(question.find("||||") + 4, question.length() - 1);

            std::stringstream q1(question1);
            std::stringstream q2(question2);
            std::string word;
            std::vector<std::string> words1;
            std::vector<std::string> words2;
            std::unordered_map<std::string, std::vector<int>> mmap;
            bool swtch = false;

            while (q1 >> word) {
                std::string str = word;
                double highestSim = 0;

                if (!swtch)
                    for (const std::string& i : allWords) {
                        double temp = getSimilarity(word, i);

                        if (temp > highestSim) {
                            highestSim = temp;
                            str = i;
                        }
                    }

                swtch = true;
                words1.push_back(str);
            }

            int ind = 0;

            while (q2 >> word) {
                words2.push_back(word);
                mmap[word].push_back(ind);
                ind++;
            }

            int rounded = compareTwoStrings(words1, words2, mmap, allWords);

            if (rounded < 10) {
                std::cout << "00" << rounded;
            }
            else if (rounded < 100) {
                std::cout << "0" << rounded;
            }
            else {
                std::cout << "100";
            }

            std::cout << std::endl;

            return 0;
        }

        std::stringstream s(question);
        std::string word;
        std::vector<std::string> words;
        std::string answer;
        std::vector<std::pair<std::vector<std::string>, int>> checkableAnswers;
        std::multimap<double, std::pair<int, std::string>> orderedList;
        double biggest = 0.0;

        while (s >> word) {
            words.push_back(word);
        }

        if (words.size() < 3) {
            std::cout << "0" << std::endl;
            return 0;
        }

        int allowedSize = words.size();

        std::string lookingForWord;

        for (std::string w: allWords) {
            double sim = getSimilarity(w, words[0]);
            if (sim > biggest) {
                biggest = sim;
                lookingForWord = w;
            }
        }

        if (stopWords.find(lookingForWord) != stopWords.end()) {
            int answersInd = 0;
            for (auto ind: stopWords[lookingForWord]) {
                for (int i = 0; i < ind.second.size(); i++) {
                    checkableAnswers.emplace_back();
                    for (int j = 0; j < allowedSize && (ind.second[i] + j < questionsList[ind.first].size()); j++) {
                        checkableAnswers[answersInd].first.push_back(questionsList[ind.first][ind.second[i] + j]);
                        checkableAnswers[answersInd].second = ind.first;
                    }
                    answersInd++;
                }
            }

            for (auto posAnswer: checkableAnswers) {
                double totalSim = 0;
                for (int i = 0; i < words.size() && i < posAnswer.first.size(); i++) {
                    totalSim += getSimilarity(words[i], posAnswer.first[i]);
                }
                if (totalSim > bestSim) {
                    bestSim = totalSim;
                    bestAnswer = map[posAnswer.second].begin()->second;
                }
                totalSim /= allowedSize;
                orderedList.insert({totalSim, {posAnswer.second, map[posAnswer.second].begin()->second}});
            }
            bestSim /= allowedSize;
        }

        if (bestAnswer.empty()) {
            std::cout << "0" << std::endl;
            return 0;
        }

        std::multimap<double,std::pair<int, std::string>>::reverse_iterator rit;
        for (rit = orderedList.rbegin(); rit != orderedList.rend(); ++rit) {
            int rounded = int(rit->first * 100);

            if (rounded < 10) {
                std::cout << "00" << rounded << rit->second.first + 1;
            }
            else if (rounded < 100) {
                std::cout << "0" << rounded << rit->second.first + 1;
            }
            else {
                std::cout << "100" << rit->second.first + 1;
            }
            std::cout << "|";
        }
        std::cout << std::endl;
    //}
}
