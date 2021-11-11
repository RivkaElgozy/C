# C
Files: TweetsGeneration.c,Tweets.txt ,README.txt (this file)
Input:
1.number of tweets to create
2.path to the text file (use the Tweets.txt text)
3.optional - nuber of words that should be readen from the text file , without this number the whole text file will be readen
Output: tweets will be printed according to the recieved quantity (argument 1), this way: Tweet {etration number}: generate sentence
Compile:gcc TweetsGenerator.c -o TweetsGenerator 
Run: ./TweetsGenerator
Remarks: 
Any two words will be separated by one or more spaces.
The last character in each line will always be the dot character (a word ending in the line may appear).
Each word cannot be longer than MAX_WORD_LENGTH(100) characters.
Line length will not exceed MAX_SENTNCE_LENGTH(1000) characters.
The file shall contain at least one sentence corresponding to at least 2 words
