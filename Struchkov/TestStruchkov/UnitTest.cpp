#include "stdafx.h"
#include "SumTree.h"
using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace TestStruchkov
{
	[TestClass]
	public ref class UnitTest
	{
	public: 
		/* Тест лагорифма по основанию 2*/
		[TestMethod]
		void TestLog2()
		{
			Assert::AreEqual(3, log2(8)); // (предполагаемый, полученный на деле)
		}
		
		/* Тест пустого дерева*/
		[TestMethod]
		void TestNullTree()
		{
			Assert::AreEqual(0.0, sum_serial(NULL)); 
		}

		/* Тест дерева из единственного узла*/
		[TestMethod]
		void TestSingleTree()
		{
			tnode node = {5.0,0.0,NULL,NULL};
			Assert::AreEqual(0.0, sum_serial(&node)); 
		}

		/* Тест последовательного рассчета суммы дерева*/
		[TestMethod]
		void TestSumSerial()
		{
			// строим дерево
			tnode* root = NULL;
			root = addNode(10,root);		// корень 10
			addNode(5,root);				// узел 5
			addNode(15,root);				// узел 15
			addNode(7,root);				// узел 7
			addNode(20.3,root);				// узел 20.3
			double result = 5+15+7+20.3;	// сумма потомков
			Assert::AreEqual(result, sum_serial(root));
			freeTree(root);
		}

		/* Тест параллельного рассчета суммы дерева (sections)*/
		[TestMethod]
		void TestSumParallel()
		{
			// строим дерево
			tnode* root = NULL;
			root = addNode(10,root);		// корень 10
			addNode(5,root);				// узел 5
			addNode(15,root);				// узел 15
			addNode(7,root);				// узел 7
			addNode(20.3,root);				// узел 20.3
			double result = 5+15+7+20.3;	// сумма потомков
			Assert::AreEqual(result, sum_parallel_section(root));
			freeTree(root);
		}

		/* Тест параллельного рассчета суммы дерева (pthread)*/
		[TestMethod]
		void TestSumParallelPthread()
		{
			// строим дерево
			tnode* root = NULL;
			root = addNode(10,root);		// корень 10
			addNode(5,root);				// узел 5
			addNode(15,root);				// узел 15
			addNode(7,root);				// узел 7
			addNode(20.3,root);				// узел 20.3
			double result = 5+15+7+20.3;	// сумма потомков
			ptArg arg;
			arg.tree = root;
			arg.nthreads = 2;
			arg.sum = 0.0;
			sum_parallel_pthread((void *) &arg);
			Assert::AreEqual(result, arg.sum);
			freeTree(root);
		}
	};
}
