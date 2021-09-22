/*
 * File:   main.cpp
 * Author: Matthew Borja
 * Purpose: Generate a script for the purpose of auto-updating steamcmd servers.
 * Created on September 20, 2021, 11:08 PM
 */

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;
const int NAME_SIZE = 20;

struct GamesInfo
{
    char name[NAME_SIZE];
    int appId;
};

void getInfo(int&, string&, string&);
bool database();
void generate(int&, string&, string&);
void generate(GamesInfo&);
long byteNum(int);
void newDatabase();



int main(int argc, char** argv) {
    char choice;                    //Store user choice from main menu.
    int appId;                      //Store manual entry app id.
    string fileName, installDir;    //Store manual entry filename and install directory.
    //Display the menu and get choice.
    cout << "SteamCMD Script Generator\n"
        << "\tMain Menu\n"
        << "1. Create New Script.\n"
        << "2. Exit.\n";
    cin >> choice;
    if (choice == 49) {             //1 will take to next step.
        getInfo(appId, fileName, installDir);
    }
    else if (choice == 42) {        //* brings up database generator.
        newDatabase();
    }
    return 0;
}

bool database() {
    ifstream database;      //Store & manipulate file.
    database.open("database.dat", ios::in | ios::binary);    //Open for binary input.
    if (database.fail()) {  //database.dat does not exist! Proceed plan b.
        return false;
    } else {  //Success!
        database.close();
        return true;
    }
}

long byteNum(int gameNum) {
    return sizeof(GamesInfo) * gameNum;     //Return the size of a GamesInfo entry.
}

void getInfo(int& appId, string& fileName, string& installDir) {
    char choice;
    int gameChoice;
    cout << "1. Retrieve from Database.\n"
         << "2. Enter manually.\n";
    cin >> choice;
    //If database exists & user chose option 1: read from file necessary data.
    if (database() && choice == 49) {
        int counter = 0;    //Store number of iterations.
        ifstream database;  //File object for input.
        GamesInfo games;    //Store GamesInfo objects from file.
        cout << "Games:\n";
        database.open("database.dat", ios::in | ios::binary);   //Open file for binary input.
        for (int i = 0; i < 7; i++) {   //Loop through game list.
            //Read an entry from database.dat & store in local object.
            database.read(reinterpret_cast<char*>(&games), sizeof(games));
            //Display in a numbered list.
            cout << counter+1 << ". " << games.name << endl;
            counter++;
        }
        do{ //Get user choice
            cout << "Select from list: ";
            cin >> gameChoice;
            //Bounds checking
            if (gameChoice == 0 || gameChoice > 7)
                cout << "Invalid choice!\n";
        } while (gameChoice==0 || gameChoice > 7);        
        database.seekg(byteNum(gameChoice-1), ios::beg);    //Move to location of game entry in database file.
        database.read(reinterpret_cast<char*>(&games), sizeof(games));  //Store data into games object.
        database.close();   //Close the file.
        generate(games);    //Pass object by reference.
    }   //Else, proceed with manual entry of data.
    else {
        cout << "Enter a file name: ";
        cin >> fileName;
        fileName += ".txt"; //Append file extension.
        cout << "Enter name of server folder: ";
        cin >> installDir;
        cout << "Enter app id: ";
        cin >> appId;
        generate(appId, fileName, installDir);  //Manual entry version of generate function.
    }
}

void generate(int& appId, string& fileName, string& installDir) {
    fstream newScript;      //Storing & manipulate files

    cout << "Generating new script. . .\n";
    //Open a new file for output.
    newScript.open(fileName, ios::out);
    newScript << "@ShutdownOnFailedCommand 1\n"
        << "@NoPromptForPassword 1\n"
        << "login anonymous\n"
        << "force_install_dir ../" << installDir << endl
        << "app_update " << appId << " validate\n"
        << "quit";
    newScript.close();  //Close the file.
    cout << "Done.\n";
}

void generate(GamesInfo& game) {
    fstream newScript;          //Storing & manipulate files
    string fileName;            //Store easy-to-use text for writing to file.
    fileName.append(game.name); //Copy c-string to string.
    fileName += ".txt";         //Append file extension.
    cout << "Generating new script. . .\n";
    //Open a new file for output.
    newScript.open(fileName, ios::out);
    //Write to new file following data:
    newScript << "@ShutdownOnFailedCommand 1\n"
        << "@NoPromptForPassword 1\n"
        << "login anonymous\n"
        << "force_install_dir ../" << game.name << endl
        << "app_update " << game.appId << " validate\n"
        << "quit";
    newScript.close();  //Close the file.
    cout << "Done.\n";
}

void newDatabase() {
    char again;         //Store user choice.
    GamesInfo newGame;  //Instantiate new GamesInfo object.
    //Open a new file for binary output.
    fstream newDatabase("database.dat", ios::out | ios::binary);
    cout << "Creating new database. . .\n";
    do {
        //Get data about a new game entry.
        cout << "New Game Entry:\n"
            << "Game Name: ";
        cin >> newGame.name;
        cout << "App ID: ";
        cin >> newGame.appId;
        //Write the contents of newGame to the file.
        newDatabase.write(reinterpret_cast<char*>(&newGame), sizeof(newGame));
        //Determine whether user wants to write another record.
        cout << "Do you want to enter another record? ";
        cin >> again;
        cin.ignore();   //Skip over the remaining newline.
    } while (again == 'Y' || again == 'y');
    //Close the file.
    newDatabase.close();
}