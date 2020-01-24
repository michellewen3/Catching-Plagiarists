#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <math.h>

using namespace std;

int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
            files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int hashing(string key, int tableSize){
    int keySize = key.length();

    unsigned int sum = 0;
    for(int i = 0; i < keySize; i++){
        sum += int(key[keySize - i - 1] * pow(3, i));
    }
    sum = sum % tableSize;
    //cout << sum << endl;
    return sum;
}

typedef struct node {
    int data;
    struct node *next;
} node;

typedef struct similarFiles {
    int fileIndex1;
    int fileIndex2;
    int numSame;
} similarFiles;

bool compareByCollisions(const similarFiles &a, const similarFiles &b)
{
    return a.numSame > b.numSame;
}

int main(int argc, char *argv[])
{
    string pathName = argv[1];

    int pathLength = pathName.length();
    if(pathName[pathLength-1] != '/'){
        pathName += '/';
    }

    int n = atoi(argv[2]);          //# chunks
    int numCopies = atoi(argv[3]);          //# of similarities

    int hashTableSize = 506573;
    int fileIndex = 0;
    //int numCopies = 200;

    //int n = 6;
    node *hashTable[hashTableSize];

    for (int i = 0; i < hashTableSize; i++){
        hashTable[i] = NULL;
    }

    //string dir = string("\\Users\\Austin\\Documents\\UT Austin\\3Sophomore-1\\EE 312- Software Design and Implementation I\\Programming Assignments\\8_Cheaters\\sm_doc_set\\");
    string dir = pathName.c_str();
    vector<string> files = vector<string>();

    getdir(dir,files);
    files.erase(files.begin());     //Erase the first .
    files.erase(files.begin());     //Erase the ..

    unsigned int size = files.size();

    int fileSize = files.size();
    int *array[fileSize];           //2D Array
    for(int i = 0; i < fileSize; i++){
        array[i] = new int[fileSize];
    }

    for(int i = 0; i < fileSize; i++){
        for(int j = 0; j < fileSize; j++){
            array[i][j] = 0;
        }
    }

    while(fileIndex < files.size()) {
        vector<string> chunks;
        vector<string> strChunks;

        string line;
        string addedWords = "";
        vector<string> text;        //Vector containing all of file's text in string format
        const char *chars;          //Array of chars holding the text
        ifstream myfile;

        myfile.open((pathName + files[fileIndex]).c_str());

        if (myfile.is_open()) {
            int i = 0;
            while (i < n) {
                getline(myfile, line, ' ');

                vector<char> charVector;
                chars = line.c_str();       //chars has
                while (*chars != '\0') {
                    if ((*chars >= 65 && *chars <= 90) || (*chars >= 48 && *chars <= 57)) {
                        charVector.push_back(*chars);
                    } else if (*chars >= 97 && *chars <= 122) {
                        charVector.push_back(*chars - 32);
                    }
                    chars++;
                }

                std::string sanitizedLine(charVector.begin(), charVector.end());

                chunks.push_back(sanitizedLine);
                i++;

            }
            for (int j = 0; j < n; j++) {
                //cout << chunks[j];
                addedWords += chunks[j];
            }

            strChunks.push_back(addedWords);


            while (getline(myfile, line, ' ')) {

                line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
                vector<string>::iterator it;
                it = chunks.begin();
                chunks.erase(it);

                //Reading in char by char to sanitize string
                vector<char> charVector;
                chars = line.c_str();
                while (*chars != '\0') {
                    if ((*chars >= 65 && *chars <= 90) || (*chars >= 48 && *chars <= 57)) {
                        charVector.push_back(*chars);
                    } else if (*chars >= 97 && *chars <= 122) {
                        charVector.push_back(*chars - 32);
                    }
                    chars++;
                }

                std::string sanitizedLine(charVector.begin(), charVector.end());

                chunks.push_back(sanitizedLine);
                addedWords = "";
                for (it = chunks.begin(); it != chunks.end(); it++) {

                    addedWords += *it;
                }
                strChunks.push_back(addedWords);

            }


            vector<string>::iterator it;

            int hashTableIndex;
            node *traversePointer;

            for (it = strChunks.begin(); it != strChunks.end(); it++) {
                hashTableIndex = hashing(*it, hashTableSize);
                traversePointer = hashTable[hashTableIndex];

                bool isInVector = false;

                while (traversePointer != NULL) {
                    if (traversePointer->data == fileIndex) {
                        isInVector = true;
                    }
                    traversePointer = traversePointer->next;
                }

                if (isInVector == false) {
                    traversePointer = hashTable[hashTableIndex];
                    node *temp = new node;
                    temp->data = fileIndex;
                    temp->next = traversePointer;
                    hashTable[hashTableIndex] = temp;
                }


            }

        }
        myfile.close();


        fileIndex++;
    }

    //populating 2D array
    node *traversePtr;
    for(int j = 0; j < hashTableSize; j++){
        traversePtr = hashTable[j];
        if(hashTable[j] != NULL) {
            while (hashTable[j]->next != NULL) {
                while (traversePtr->next != NULL) {
                    traversePtr = traversePtr->next;
                    array[hashTable[j]->data][traversePtr->data]++;
                }
                traversePtr = hashTable[j];
                hashTable[j] = hashTable[j]->next;
                delete traversePtr;
                traversePtr = hashTable[j];
            }
            delete hashTable[j];
        }
    }

    //Finds collisions greater than numCopies and puts into a vector
    vector<similarFiles> collisions;
    for(int i = 0; i < fileSize; i++){
        for(int j = 0; j < fileSize; j++){
            //cout << array[i][j] << " ";
            if(array[i][j] > numCopies){
                similarFiles pair;
                pair.fileIndex1 = i;
                pair.fileIndex2 = j;
                pair.numSame = array[i][j];
                collisions.push_back(pair);
            }
        }
        //cout << endl;
    }

    //Sort vector
    std::sort(collisions.begin(), collisions.end(), compareByCollisions);

    vector<similarFiles>::iterator it;
    for (it = collisions.begin(); it != collisions.end(); it++) {
        cout << it->numSame << ": " << files[it->fileIndex1] << ", " << files[it->fileIndex2] << endl;
    }

    for(int i = 0; i < fileSize; i++){
        delete[] array[i];
    }

    return 0;
}

//Check if document has less words than chunk length