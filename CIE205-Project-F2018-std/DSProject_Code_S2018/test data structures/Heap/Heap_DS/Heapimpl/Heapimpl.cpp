
#include "stdafx.h"
#include <iostream>
#include "Heap.h"

using namespace std;
int main()
{
	Heap<int>* h = new Heap<int>(50);
	for (int i=-50;i<0;i++)
	{
		h->Enqueue(i, i);
	}
	for (int i = -50; i < 0; i++)
	{
		h->Enqueue(i, i);
	}
	for (int i = -50; i < 0; i++)
	{
		cout<<h->Dequeue()->getdata()<<endl;
	}
	for (int i = -50; i < 0; i++)
	{
		cout << h->Dequeue()->getdata() << endl;
	}

}