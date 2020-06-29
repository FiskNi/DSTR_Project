#ifndef _DEBUGOBJECTS_H
#define _DEBUGOBJECTS_H
#include <Pch/Pch.h>

struct DebugFractureTimer
{
	struct MicroResult
	{
		long long time;
		int size;

		MicroResult(long long t, int s)
		{
			time = t;
			size = s;
		}
	};
	struct MilliResult
	{
		float time;
		int size;

		MilliResult(float t, int s)
		{
			time = t;
			size = s;
		}
	};

	// Total results
	std::vector<MicroResult> averageComputation_micro;
	std::vector<MilliResult> averageComputation_milli;

	std::vector<MicroResult> averageGPU_micro;
	std::vector<MilliResult> averageGPU_milli;

	std::vector<MicroResult> averageDiagram_micro;
	std::vector<MilliResult> averageDiagram_milli;

	std::vector<MicroResult> averageMeshes_micro;
	std::vector<MilliResult> averageMeshes_milli;


	float averageTotal_micro = 0;
	float averageTotal_milli = 0;
	float time = 0;

	// Iteration
	int currentIteration = 0;
	int maxIterations = 50;

	int currentSize = 4;
	int maxSize = 1000;

	// Current iteration results

	std::vector<long long>	computationTime_micro;
	std::vector<float>		computationTime_milli;

	std::vector<long long>	gpuTime_micro;
	std::vector<float>		gpuTime_milli;

	std::vector<long long>	diagramTime_micro;
	std::vector<float>		diagramTime_milli;

	std::vector<long long>	meshesTime_micro;
	std::vector<float>		meshesTime_milli;

	void Allocate()
	{
		size_t size = (size_t)maxSize - (size_t)currentSize;
		averageComputation_micro.reserve(size);
		averageComputation_milli.reserve(size);
		averageGPU_micro.reserve(size);
		averageGPU_milli.reserve(size);
		averageDiagram_micro.reserve(size);
		averageDiagram_milli.reserve(size);
		averageMeshes_micro.reserve(size);
		averageMeshes_milli.reserve(size);


		computationTime_micro.reserve((size_t)maxIterations);
		computationTime_milli.reserve((size_t)maxIterations);
		gpuTime_micro.reserve((size_t)maxIterations);
		gpuTime_milli.reserve((size_t)maxIterations);
		diagramTime_micro.reserve((size_t)maxIterations);
		diagramTime_milli.reserve((size_t)maxIterations);
		meshesTime_micro.reserve((size_t)maxIterations);
		meshesTime_milli.reserve((size_t)maxIterations);
	}

	void IterationAllocate()
	{
		computationTime_micro.reserve((size_t)maxIterations);
		computationTime_milli.reserve((size_t)maxIterations);
		gpuTime_micro.reserve((size_t)maxIterations);
		gpuTime_milli.reserve((size_t)maxIterations);
		diagramTime_micro.reserve((size_t)maxIterations);
		diagramTime_milli.reserve((size_t)maxIterations);
		meshesTime_micro.reserve((size_t)maxIterations);
		meshesTime_milli.reserve((size_t)maxIterations);
	}


	void ComputeAverage()
	{
		long long	avc_micro = 0;
		float		avc_milli = 0;

		long long	avp_micro = 0;
		float		avp_milli = 0;

		long long	avd_micro = 0;
		float		avd_milli = 0;

		long long	avm_micro = 0;
		float		avm_milli = 0;


		long long r_micro = 0;
		int d = 0;
		for (long long t : computationTime_micro)
		{
			r_micro += t;
			d++;
		}
		avc_micro = r_micro / d;

		float r_milli = 0;
		d = 0;
		for (float t : computationTime_milli)
		{
			r_milli += t;
			d++;
		}
		avc_milli = r_milli / d;




		r_micro = 0;
		d = 0;
		for (long long t : gpuTime_micro)
		{
			r_micro += t;
			d++;
		}
		avp_micro = r_micro / d;

		r_milli = 0;
		d = 0;
		for (float t : gpuTime_milli)
		{
			r_milli += t;
			d++;
		}
		avp_milli = r_milli / d;





		r_micro = 0;
		d = 0;
		for (long long t : diagramTime_micro)
		{
			r_micro += t;
			d++;
		}
		avd_micro = r_micro / d;

		r_milli = 0;
		d = 0;
		for (float t : diagramTime_milli)
		{
			r_milli += t;
			d++;
		}
		avd_milli = r_milli / d;




		r_micro = 0;
		d = 0;
		for (long long t : meshesTime_micro)
		{
			r_micro += t;
			d++;
		}
		avm_micro = r_micro / d;

		r_milli = 0;
		d = 0;
		for (float t : meshesTime_milli)
		{
			r_milli += t;
			d++;
		}
		avm_milli = r_milli / d;



		averageComputation_micro.emplace_back(avc_micro, currentSize);
		averageComputation_milli.emplace_back(avc_milli, currentSize);
		averageGPU_micro.emplace_back(avp_micro, currentSize);
		averageGPU_milli.emplace_back(avp_milli, currentSize);
		averageDiagram_micro.emplace_back(avd_micro, currentSize);
		averageDiagram_milli.emplace_back(avd_milli, currentSize);
		averageMeshes_micro.emplace_back(avm_micro, currentSize);
		averageMeshes_milli.emplace_back(avm_milli, currentSize);
	}

	void ComputeTotal()
	{
		long long result_micro = 0;
		int d = 0;
		for (MicroResult microRes : averageComputation_micro)
		{
			result_micro += microRes.time;
			d++;
		}
		averageTotal_micro = result_micro / d;

		float result_milli = 0;
		d = 0;
		for (MilliResult milliRes : averageComputation_milli)
		{
			result_milli += milliRes.time;
			d++;
		}
		averageTotal_milli = result_milli / d;
	}

	void ClearCurrent()
	{
		// Clear current iteration
		computationTime_micro.clear();
		computationTime_milli.clear();
		gpuTime_micro.clear();
		gpuTime_milli.clear();
		diagramTime_micro.clear();
		diagramTime_milli.clear();
		meshesTime_micro.clear();
		meshesTime_milli.clear();


		// Clear current itteration
		currentIteration = 0;
	}

	bool CheckSize()
	{
		if (currentSize <= maxSize)
			return true;
		return false;
	}
	bool CheckIteration()
	{
		if (currentIteration <= maxIterations)
			return true;
		return false;
	}

	void Reset()
	{
		ClearCurrent();
		// Total results
		averageComputation_micro.clear();
		averageComputation_milli.clear();
		averageGPU_micro.clear();
		averageGPU_milli.clear();
		averageDiagram_micro.clear();
		averageDiagram_milli.clear();
		averageMeshes_micro.clear();
		averageMeshes_milli.clear();

		int currentSize = 5;
	}



	void WriteResults()
	{
		std::ofstream asciiFile;
		asciiFile.open("Compute_Results_Micro.txt");
		for (MicroResult microRes : averageComputation_micro)
		{
			asciiFile << microRes.time << std::endl;
		}
		asciiFile.close();


		asciiFile.open("GPU_Results_Micro.txt");
		for (MicroResult microRes : averageGPU_micro)
		{
			asciiFile << microRes.time << std::endl;
		}
		asciiFile.close();



		asciiFile.open("Diagram_Results_Micro.txt");
		for (MicroResult microRes : averageDiagram_micro)
		{
			asciiFile << microRes.time << std::endl;
		}
		asciiFile.close();



		asciiFile.open("Meshes_Results_Micro.txt");
		for (MicroResult microRes : averageMeshes_micro)
		{
			asciiFile << microRes.time << std::endl;
		}
		asciiFile.close();



	}



};

struct Weapon
{
	float cooldown = 0.0f;
	float cooldown_fast = 0.0f;

	float spread = 0.0f;

	float spread_low = 0.0f;
	float spread_wide = 0.05f;
	float fireRate = 0.1f;		// Slow
	float fireRate_fast = 0.02f;	// Fast

	glm::vec3 position;
	glm::vec3 direction;


	void Update(float deltaTime)
	{
		cooldown -= deltaTime;
		cooldown_fast -= deltaTime;
	}

	void Shot()
	{
		cooldown = fireRate;
	}

	bool CanShoot()
	{
		if (cooldown <= 0)
		{
			return true;
		}

		return false;
	}

	bool Shoot()
	{
		if (Input::isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (cooldown <= 0)
			{
				spread = spread_low;
				cooldown = fireRate;
				return true;
			}
		}

		if (Input::isMouseHeldDown(GLFW_MOUSE_BUTTON_4))
		{
			if (cooldown_fast <= 0)
			{
				spread = spread_wide;
				cooldown_fast = fireRate_fast;
				return true;
			}
		}

		return false;
	}

	bool Pierce()
	{
		if (Input::isMousePressed(GLFW_MOUSE_BUTTON_5))
		{
			if (cooldown <= 0)
			{
				spread = spread_low;
				cooldown = fireRate;
				return true;
			}
		}


		return false;
	}


};

#endif