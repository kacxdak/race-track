#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "main.h"
#include "game.h"
#include "reinforce.h"

using namespace std;

Reinforce::Reinforce( Game* curr_game, int epsilon ) {
	game = curr_game;
	e = epsilon;
	
	int width = curr_game->Width();
	int height = curr_game->Height();
	int vel = VELMAX - VELMIN;
	
	//initialize rewards to 0
	reward = new double***[width];
	for( int i=0; i<width; i++ ) {
		reward[i] = new double**[height];
		for( int j=0; j<height; j++ ) {
			reward[i][j] = new double*[vel];
			for( int k=0; k<vel; k++ ) {
				reward[i][j][k] = new double[vel];
				for ( int l=0; l<vel; l++ )
					reward[i][j][k][l] = 0.0;
			}
		}
	}
	visits = new int***[width];
	for( int i=0; i<width; i++ ) {
		visits[i] = new int**[height];
		for( int j=0; j<height; j++ ) {
			visits[i][j] = new int*[vel];
			for( int k=0; k<vel; k++ ) {
				visits[i][j][k] = new int[vel];
				for ( int l=0; l<vel; l++ )
					visits[i][j][k][l] = 0;
			}
		}
	}
	
}

void Reinforce::Log( string filename ) {
	filename.append(".log");
	ofstream myfile;
	myfile.open(filename.c_str(), ios::app);
	myfile<<game->Reward()<<endl;
	myfile.close();
}

void Reinforce::Update() {
	vector<Identity> moveList = game->Moves();
	int total_reward = game->Reward();
	
	for(int curr_move=0; curr_move<moveList.size(); curr_move-- ) {
		Identity item = moveList[curr_move];
		double* curr_r = &reward[item.loc.x][item.loc.y][item.vel.up][item.vel.right];
		int* curr_v = &visits[item.loc.x][item.loc.y][item.vel.up][item.vel.right];
		
		reward[item.loc.x][item.loc.y][item.vel.up][item.vel.right] = (*curr_r)*(*curr_v) + (double) total_reward;
		visits[item.loc.x][item.loc.y][item.vel.up][item.vel.right] = (*curr_v)+1;
		reward[item.loc.x][item.loc.y][item.vel.up][item.vel.right] = (*curr_r) / ( (double) (*curr_v)+1);
	}
}

Vel Reinforce::Move( ) {
	int numb = rand() % 100;
	if( numb <= e ) {
		//cout<<"Exploring: "<<endl;
		//sleep(1);
		return Explore();
	} else {
		//cout<<"Exploiting: "<<endl;
		//sleep(1);
		return Exploit();
	}
}

void Reinforce::Epsilon( int gameCounter ) {
	if(gameCounter % 100 == 99)
		e*= 0.9;
}

//Private Functions
Vel Reinforce::Exploit( ) {
	Coor car = game->Position();
	Vel vel = game->Velocity();
	double **curr_reward = reward[car.x][car.y];
	int uStart = max(vel.up + abs(VELMIN) - 1, 0);
	int uEnd = min(vel.up + abs(VELMIN) + 1, VELMAX-VELMIN-1);
	int rStart = max(vel.right + abs(VELMIN) - 1, 0);
	int rEnd = min(vel.right + abs(VELMIN) + 1, VELMAX-VELMIN-1);
	
	vector<int> uMAX, rMAX;
	uMAX.push_back(uStart);
	rMAX.push_back(rStart);
	
	double maxReward = curr_reward[uMAX[0]][rMAX[0]]; 
	for( int i=uStart; i<=uEnd; i++ )
		for( int j=rStart; j<=rEnd; j++ )
			if(curr_reward[i][j] > maxReward) {
				maxReward = curr_reward[i][j];
				uMAX.clear();
				rMAX.clear();
				uMAX.push_back(i);
				rMAX.push_back(j);
			} else if (curr_reward[i][j] == maxReward) {
				uMAX.push_back(i);
				rMAX.push_back(j);
			}
	
	vel.up = uMAX[rand() % uMAX.size()] - abs(VELMIN);
	vel.right = rMAX[rand() % rMAX.size()] - abs(VELMIN);
	return vel;
}

Vel Reinforce::Explore( ) {
	Vel vel;
	vel.up = rand() % 3 - 1;
	vel.right = rand() % 3 - 1;
	return vel;
}
