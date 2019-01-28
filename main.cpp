#include<iostream>
#include "preprocessing_phase.h"
#include "Trivium.h"


int main()
{
	
	PreprocessingPhase<Trivium> test(80, 80);
	test.SetCubeSize(5);
	test.SetRoundNumber(671, 5);
	test.HardPreprocessing();
	
	system("pause");
	return 0;
} 