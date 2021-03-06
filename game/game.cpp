#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "main.h"
#include "game.h"

using namespace std;

Game::Game( string filename ) {
	string name = filename;
	name.append(".dat");
	ifstream fin (name.c_str());
	if(fin.is_open()) {
		fin>>height>>width;
		string temp;
		getline(fin, temp);
		track = new int*[width];
		for(int i=0; i<width; i++) {
			track[i] = new int[height];
			getline(fin, temp);
			for( int j=0; j<height; j++) {
				track[i][j] = fromGUI(temp[j]);
				if(track[i][j] == -1) {
					Coor temp;
					temp.x = i;
					temp.y = j;
					start.push_back(temp);
				}
			}
		}
		Reset();
	} else {
		cout<<"Unable to open "<<name<<endl;
		exit(0);
	}
}

void Game::Reset() {
	coor = start[rand() % start.size()];
	reward = 0;
	uVel = 0;
	rVel = 0;
	status = true;
	moveList.clear();
}

void Game::Print() {
	system("clear");
	for(int i=0; i<width; i++) {
		for( int j=0; j<height; j++) {
			cout<<((coor.x == i && coor.y == j) ? 'C' : gui(track[i][j]));
		}
		cout<<endl;
	}
    cout<<"Score: "<<reward<<endl;
	cout<<"Up: "<<uVel<<" ::: Right: "<<rVel<<endl;
}

int Game::Move(int up, int right) {
	
	uVel += up;
	rVel += right;
	
	//Make Sure it is within bounds (-5:+5)
	uVel = min(VELMAX, uVel);
	rVel = min(VELMAX, rVel);
	uVel = max(VELMIN, uVel);
	rVel = max(VELMIN, rVel);
	
	Identity item;
	item.loc = coor;
	item.vel.up = uVel;
	item.vel.right = rVel;
	int res = processMove();
	//always lose 1 for making a move
	reward -= 1;
	
	item.reward = -1;
	
	//lose additional 99 for crashing
	if(res == -5) {
		reward -= 99;
		item.reward -= 99;
	}
	
	if(res == -2)
		status = false;
	if((int) moveList.size() > 200 && status)
		item.reward -= 10000;
	moveList.push_back(item);
	
	return res;
}

//Private Functions........................................

int Game::processMove() {
	bool crash = false, crashed=false;
	
	int tempU = uVel, tempR = rVel;
//	cout<<"We start at: "<<coor.x<<", "<<coor.y<<endl;
	while((tempU != 0 || tempR != 0) && track[coor.x][coor.y] != -2 ) {
        int upward = (tempU != 0 ? tempU/abs(tempU) : 0);
        int rightward = (tempR != 0 ? tempR/abs(tempR) : 0);
		coor.x -= upward;
		coor.y += rightward;
		crash = fixCrash(upward, rightward);
		if(crash && !crashed)
			crashed = true;
		
		//move velocity towards 0
		if(tempU > 0 )
			tempU--;
		else if (tempU < 0)
			tempU++;
		if(tempR > 0 )
			tempR--;
		else if (tempR < 0)
			tempR++;
//		cout<<"We are at: "<<coor.x<<", "<<coor.y<<endl;
	}
	
	//This means we got the end
	if(track[coor.x][coor.y] == -2)
		crashed = false;
    
	if(crashed)
		return -5;
	
	return track[coor.x][coor.y];
}

void Game::halfmove() {
    bool crash;
    //Car always moves 1
    //initially test if it can move up
    coor.x -= 1;
    crash = fixCrash(1, 0);
    //if move failed, move right
    if(crash) {
        coor.y +=1;
        crash = fixCrash(0, 1);
    }
    //else move left
    if(crash) {
        coor.y -=1;
        crash = fixCrash(0, -1);
    }
}

char Game::gui(int x) {
	switch(x) {
		case 0:
			return '.';
		case -1:
			return 'S';
		case -2:
			return 'F';
		case -5:
		default:
			return '|';
	}
}

int Game::fromGUI(char x) {
	switch (x) {
		case '.':
			return 0;
		case 'S':
			return -1;
		case 'F':
			return -2;
		case '|':
		default:
			return -5;
	}
}

bool Game::fixCrash(int uMove, int rMove) {
	bool crash = false;
	//boundary checks to make sure we can access the index in the track
	if(coor.x < 0) {
		coor.x = 0;
		crash = true;
	} else if(coor.x >= width) {
		coor.x = width-1;
		crash = true;
	}
	if(coor.y < 0) {
		coor.y = 0;
		crash = true;
	} else if (coor.y >= height) {
		coor.y = height - 1;
		crash = true;
	}
	
	//fix right left
	if( track[coor.x][coor.y] == -5 ) {
		if(rMove > 0) {
			coor.y--;
			//check that we are indeed back on the track
			if(track[coor.x][coor.y] == -5)
				coor.y++;
		} else if ( rMove < 0 ) {
			coor.y++;
			//check that we are indeed back on the track
			if(track[coor.x][coor.y] == -5)
				coor.y--;
		}
		crash = true;
	}
	//fix up down
	if( track[coor.x][coor.y] == -5 ) {
		if(uMove > 0) {
			coor.x++;
			//check that we are indeed back on the track
			if(track[coor.x][coor.y] == -5)
				coor.x--;
		} else if (uMove < 0) {
			coor.x--;
			//check that we are indeed back on the track
			if(track[coor.x][coor.y] == -5)
				coor.x++;
		}
		crash = true;
	}
	//fix diagonal
	if( track[coor.x][coor.y] == -5 ) {
		coor.x -= -1*uMove;
		coor.y += -1*rMove;
		//check that we are indeed on track
		if( track[coor.x][coor.y] == -5 ) {
			coor.x -= -1*uMove;
			coor.y += -1*rMove;
		}
		crash = true;
	}
	
	return crash;
}
