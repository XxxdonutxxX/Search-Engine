# Introduction
This is a search engine that takes in a scentence with mispelled or not correct words, and goes through the file really fast, picking up the possible questions that match up with the question the user is providing. It also compares two strings and gives out the similarity between them.


# Arguments
  - Argument one is the path of the file
  - Argument two is the question


# How to format the file to be parsed
Inside the file, each new question should start with `{}{}` and separate the question from the answer (if you wanted to make it QA instead of just questions) by using `||||` and end the whole line with `{}{}`

  **Examples**
  
  `{}{}What is the fifth state in the United States||||Connecticut{}{}` => If there is an answer to the question
  
  `{}{}In what year George Washington became a president||||{}{}` => If there was no answer to the quesiton
  

# What is the input of the program
If you write one sentence, then the program looks for that sentnce in the file. But if you type `setence||||sentence`, then a comparison will be made, showing the similarity between the two setneces.

  **Examples**
  
  `Whaaaat isds the fifh=hth state in tha Unitess Statted` => Will look in the file
  
  `This is sentence one||||This is sentence two` => Will compare the two sentences and give the similarity between them
  

# What is the expected output
If you are looking for a question in the file, it will print an answer in the format `0000`, where the first 3 digits represent the similarity between the given question and the possible match question, and the last digit represents the index of the question in the file (starting from index 1). The possible matched questions are seperate with `|`. But if the input is comparing two strings, then gives an output in the format `000`, where this is the similiarity percentage. Let's take the examples from above

  **Examples**
  
  `Whaaaat isds the fifh=hth state in tha Unitess Statted` => `0751|`
  
  `This is sentence one||||This is sentence two` => `053`
  
  
# How does it work
The basic idea is that it I want the program to read the file once and store all the required information in memory, so that if I look for a word, I know exactly where to go and not read everything again. First, the file is parsed. It makes a list of all the possible words that were found in the file, then makes a map in where each words is found in which index in the sentence in which indexed question. After that, it asks the user for an input. It checks if there exists the substring `||||`. If yes, then the user is trying to compare two strings. Take the first word in string one. Clean it by running it through the list of all words and see which word matches it best, and replace it with that. Then go through the second string and clean each word. After that, look in which index in string two has the word that matches the first word in string one. Save that index, and start comparing the two strings starting from that index. Same idea in comparing the string to the inputs in the file.

  
# Notes
This is an extention of another project, that's why the file parsing is specific. This project is made overnight, when I was really sleepy. So don't be surprised if there were some bugs here and there.
