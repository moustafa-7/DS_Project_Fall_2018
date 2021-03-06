#include "Battle.h"
#include "File_IO.h"
#include "Data_strutres_in_use/Queue/Queue.h"
#include"Castle\Castle.h"
#include "Data_strutres_in_use/heap/Heap.h"
#include <iostream>
#include "Melter.h"
#include "Vanisher.h"

const int MAX_ENEMIES = 300;

Battle::Battle()
{
	EnemyCount = 0;
}

void Battle::AddEnemy(Enemy* Ptr)
{
	if (EnemyCount < MaxEnemyCount)
	{
		if (Ptr != nullptr) BEnemiesForDraw[EnemyCount] = Ptr;
		EnemyCount++;
	}

	// Note that this function doesn't allocate any enemy objects
	// It only makes the first free pointer in the array
	// points to the same enemy pointed to by "Ptr"
}


void Battle::ClearEnemy()
{
	for (int i = 0; i < EnemyCount; i++)
	{
		BEnemiesForDraw[i] = nullptr;
	}
	EnemyCount = 0;
}

Castle * Battle::GetCastle()
{
	return &BCastle;
}


void Battle::set_initlailized_castle(double health, int no_enemies, double power)
{
	BCastle.set_all_towers(health, no_enemies, power);
}



void Battle::RunSimulation()
{
	phase2_simulation();
}



int Battle::compute_priority(Enemy* ptr)
{
	if (ptr != nullptr)
	{
		double health = ptr->get_health();
		double power = ptr->get_power();
		double distance = ptr->GetDistance();
		return (0.2*health + 0.1*power + 7 / distance + 3);
	}
	else return 0;

}



Queue <Enemy*>* Battle::fill_inactivelist(Queue <int*>* Data)
{
	int enemy_id;
	int type;
	int arrival_time;
	int health;
	int pow;
	int reload;
	int region;
	int*dummyarr;
	// Accessing Enemey Classes
	Fighter * fighter;
	Healer * healer;
	Freezer * freezer;
	Melter * melter;
	Vanisher * vanisher;



	Queue <Enemy*>*inactive_enemies = new Queue <Enemy*>;//a queue that holds a pointer to enemies
	{
		int intialsize = Data->getsize();
		for (int i = 0; i < intialsize; i++)//fill inactive list
		{
			dummyarr = Data->deque();
			// Set the Tower parameters
			enemy_id = dummyarr[0];
			type = dummyarr[1];
			arrival_time = dummyarr[2];
			health = dummyarr[3];
			pow = dummyarr[4];
			reload = dummyarr[5];
			region = dummyarr[6] - 1;

			REGION reg = A_REG;


			// defining the regions as integers
			switch (region)
			{
			case 0:
				reg = A_REG;
				break;
			case 1:
				reg = B_REG;
				break;
			case 2:
				reg = C_REG;
				break;
			case 3:
				reg = D_REG;
				break;
			}

			switch (type)   // instansiating the enemies depending on their type
			{
			case 1://fighter
				fighter = new Fighter(FIGHTER_CLR, enemy_id, arrival_time, health, pow, reg, 60, 3);
				inactive_enemies->enque(fighter);

				break;
			case 2://healer
				healer = new Healer(HEALER_CLR, enemy_id, arrival_time, health, pow, reg, 60, 3);
				inactive_enemies->enque(healer);

				break;
			case 3://freezer
				freezer = new Freezer(FREEZER_CLR, enemy_id, arrival_time, health, pow, reg, 60, 3);
				inactive_enemies->enque(freezer);

				break;
			case 4: //melter
				melter = new Melter(MELTER_CLR, enemy_id, arrival_time, health, pow, reg, 60, 3);
				inactive_enemies->enque(melter);

				break;
			case 5://vanisher
				vanisher = new Vanisher(VANISHER_CLR, enemy_id, arrival_time, health, pow, reg, 60, 3);
				inactive_enemies->enque(vanisher);

				break;
			}

		}
	}
	return inactive_enemies;
}

/*Queue<Enemy*>* Battle::get_killed_enemies()
{
	return killed_enemies;
}*/

//This is just a demo function for project introductory phase
//It should be removed in phases 1&2
void Battle::phase2_simulation()
{

	Queue<int*>*Data = new  Queue<int*>;

	Data = get_file_2_queue("input_file.txt");//enter the name of the file here(optional);

	int *dummyarr = Data->deque();//remove the first line has the tower data
	double TH = dummyarr[0];//tower health
	int max_enemies = dummyarr[1];//number of enemies attacked per second
	double TP = dummyarr[2];//tower power
	int recieved_enemies = Data->getsize(); //Number of enemies in input file

	set_initlailized_castle(TH, max_enemies, TP); //Intialize castle parameters using first line of input file

// intialize queues
	Queue <Enemy*>*inactive_enemies = fill_inactivelist(Data);//a queue that holds a pointer to enemies
	Queue <Enemy*>*killed_enemies = new Queue <Enemy*>;//a queue that holds pointer to enemies

	std::cout << "\nWelcome to Castle Battle:\n";
	std::cout << "\nIn phase2, you will be asked to select game mode\n";
	std::cout << "\nFor now just press ENTER key to continue...";

	double total_tower_health = BCastle.get_total_tower_health();

	GUI * pGUI = new GUI;


	Heap<Enemy*>** Heap1 = new Heap<Enemy*>*[4];//this two heap arrays will contain all the active enemies that will alternate between each one
	Heap<Enemy*>** Heap2 = new Heap<Enemy*>*[4];

	//int a = inactive_enemies->getsize();

	for (int i = 0; i < 4; i++)//intialinzing the two heaps
	{
		Heap1[i] = new Heap<Enemy*>(recieved_enemies);
		Heap2[i] = new Heap<Enemy*>(recieved_enemies);
	}

	double tower_1_health = BCastle.get_tower(0)->GetHealth();
	double tower_2_health = BCastle.get_tower(1)->GetHealth();
	double tower_3_health = BCastle.get_tower(2)->GetHealth();
	double tower_4_health = BCastle.get_tower(3)->GetHealth();



	int current_tick = 0;

	Heap<Enemy*>** current_heap = Heap1;//those variabel names are used for readability and to swap the two main heap arrays easily
	Heap<Enemy*>** to_be_filled_heap = Heap2;
	Heap<Enemy*>** temp_heap;

	int region_index;
	int enemey_priority;
	bool activationflag = true;
	Enemy* current_enemy;

	Enemy** to_be_hit_enemies = new Enemy*[max_enemies];//this array is made to have only the enemies that should be hit by the tower

	Point p;

	int current_heap_number;

	int no_killed_enemies[4] = { 0,0,0,0 };


	while (killed_enemies->getsize() < recieved_enemies || total_tower_health != 0)//this loop will end when all the towers are destroyed or all the enemies are killed
	{
		ClearEnemy();//leans the drawing area to redraw agin in the currennt loop

		activationflag = true;

		while (!inactive_enemies->isEmpty() && activationflag) // this loop takes out all the enemies that should enter the battle ni the curreent tick
			// no two enemies enter the the same region at the same time
		{
			/*
			for (int j = 0; j < max_enemies; j++)
			{
				to_be_hit_enemies[j] = nullptr;
			}
			*/

			if (inactive_enemies->front()->get_arraival_time() == current_tick)
			{
				inactive_enemies->front()->set_FD(current_tick);
				region_index = inactive_enemies->front()->GetRegion();
				enemey_priority = compute_priority(inactive_enemies->front());
				current_heap[region_index]->Enqueue(enemey_priority, inactive_enemies->deque());
			}
			else
			{
				activationflag = false;
			}
		}

		current_tick++;

			//====== =
			current_tick++;

		for (int i = 0; i < 4; i++)//one iteration per tower, this loop is made to make the enemies act on the tower
		{
			current_heap_number = current_heap[i]->getcurrent_number();
			if (current_heap_number != 0)
			{


			for (int i = 0; i < 4; i++)//one iteration per tower, this loop is made to make the enemies act on the tower
			{
				current_heap_number = current_heap[i]->getcurrent_number();
				if (current_heap_number != 0)
				{

					int current_heap_size = current_heap[i]->getcurrent_number();

					for (int j = 0; j < current_heap_size; j++)
					{
						to_be_filled_heap[i]->Enqueue(compute_priority(current_enemy), current_enemy);
						current_enemy->set_target(BCastle.get_tower(i));
						current_enemy->take_heap(current_heap[i]);
						current_enemy->Act();
						AddEnemy(current_enemy);
						current_enemy->Move();
					}

						current_enemy = current_heap[i]->Dequeue();
						if (current_enemy != nullptr)
						{
							//<<<<<<< HEAD
							//=======
							to_be_filled_heap[i]->Enqueue(compute_priority(current_enemy), current_enemy);
							//>>>>>>> 934170096bb1e0f9d6a60ac4bd525eb535440725
							current_enemy->set_target(BCastle.get_tower(i));
							current_enemy->take_heap(current_heap[i]);
							current_enemy->Act();
							AddEnemy(current_enemy);
							current_enemy->Move();
						}


						// Printing the tower data
						tower_1_health = BCastle.get_tower(0)->GetHealth();
						tower_2_health = BCastle.get_tower(1)->GetHealth();
						tower_3_health = BCastle.get_tower(2)->GetHealth();
						tower_4_health = BCastle.get_tower(3)->GetHealth();


						string messege = " TH1:(" + to_string(tower_1_health) + ")TH2:(" + to_string(tower_2_health) + ")TH3:(" + to_string(tower_3_health) + ")TH4:(" + to_string(tower_4_health) + ")";
						string messege2 = "TE1:(" + to_string(to_be_filled_heap[0]->getcurrent_number()) + ")TE2:(" + to_string(to_be_filled_heap[1]->getcurrent_number()) + ")TE3:(" + to_string(to_be_filled_heap[2]->getcurrent_number()) + ")TE4:(" + to_string(to_be_filled_heap[3]->getcurrent_number()) + ")";
						string messege3 = "TK1:(" + to_string(no_killed_enemies[0]) + ")TK2:(" + to_string(no_killed_enemies[1]) + ")TK3:(" + to_string(no_killed_enemies[2]) + ")TK4:(" + to_string(no_killed_enemies[3]) + ")";
						string messege4 = "----------------------CT " + to_string(current_tick - 1);
						messege = messege + messege2 + messege3 + messege4;

						pGUI->PrintMessage(messege);
					}

					// vanisher functions
					int vanishing_time = current_enemy->get_vanishing_time();
					if (current_enemy->get_type() == 5) {
						current_enemy->increment_vanishing_time(); // counter
						// From tick 0:4, vanisher is visible.
						if (current_enemy->get_vanishing_time() == 5 + vanishing_time) // From tick 5:9, vanisher is visible.
							current_enemy->set_visible(0);
						if (current_enemy->get_vanishing_time() == 10 + vanishing_time) // repeat vanishing and appearing for next ticks.
						{
							current_enemy->set_visible(1);
							current_enemy->set_vanishing_time(0);
						}

					}

					killed_enemies->enque(current_enemy);

					pGUI->DrawBattle(BEnemiesForDraw, EnemyCount); //we draw in here because an enemy can exist and get killed in the same tick
					for (int j = 0; j < max_enemies; j++)//this loop kills the enemies
					{
						if (to_be_filled_heap[i]->getcurrent_number() > 0 && current_enemy->get_visible() == 1)
						{
							to_be_hit_enemies[j] = to_be_filled_heap[i]->Dequeue();

							if (to_be_hit_enemies[j] != nullptr)
								BCastle.tower_act(i, to_be_hit_enemies[j], current_tick);

							if (to_be_hit_enemies[j] != nullptr)
							{
								if (to_be_hit_enemies[j]->is_killed())
								{
									to_be_hit_enemies[j]->set_KTS(current_tick - 1);
									to_be_hit_enemies[j]->set_KD((current_tick - 1 - to_be_hit_enemies[j]->get_tfirst_shot()));
									to_be_hit_enemies[j]->set_LT();
									killed_enemies->enque(to_be_hit_enemies[j]);
									to_be_hit_enemies[j] = nullptr;
									no_killed_enemies[j]++;
									//current_heap[i]->Enqueue(0,nullptr);
								}
								else
								{
									current_heap[i]->Enqueue(compute_priority(to_be_hit_enemies[j]), to_be_hit_enemies[j]);
								}
							}
						}
					}
				}

			}

		int size_khaled = killed_enemies->getsize();





		BCastle.reconstruct_towers();

		//pGUI->GetPointClicked(p);
		Sleep(1000);
		temp_heap = current_heap;
		current_heap = to_be_filled_heap;
		to_be_filled_heap = temp_heap;
		if (tower_1_health < 1 && tower_2_health < 1 && tower_3_health < 1 && tower_4_health < 1)
		{
			BCastle.SetTowerHealth(A_REG, 0);
			BCastle.SetTowerHealth(B_REG, 0);
			BCastle.SetTowerHealth(C_REG, 0);
			BCastle.SetTowerHealth(D_REG, 0);

		}

		tower_1_health = BCastle.get_tower(0)->GetHealth();
		tower_2_health = BCastle.get_tower(1)->GetHealth();
		tower_3_health = BCastle.get_tower(2)->GetHealth();
		tower_4_health = BCastle.get_tower(3)->GetHealth();

		int output = 0;

		//if (tower_1_health + tower_2_health + tower_3_health + tower_4_health == 0)
		//{
		//	if (killed_enemies->getsize() == recieved_enemies)//tie
		//	{
		//		output = 2;
		//		break;
		//	}
		//	else //enemies win (towers died)
		//	{
		//		output = 3;
		//		break;
		//	}
		//}
		//else if (killed_enemies->getsize() == recieved_enemies)// all enemies killed
		//{
		//	output = 1;
		//	break;
		//}


	}


	//delete pGUI;




	//	// output file
	//ofstream myfile;
	//myfile.open("output_file.txt");
	//myfile << "KTS S FD KD LT\n";
	//for (int i = 0; i < killed_enemies->getsize(); i++)
	//{
	//	Enemy* enemy = killed_enemies->deque();
	//	myfile << enemy->get_KTS() << " " << i + 1 << enemy->get_FD() << enemy->get_KD() << enemy->get_LT();
	//}
	//myfile.close();


	}
