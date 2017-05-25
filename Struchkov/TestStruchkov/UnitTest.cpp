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
		/* ���� ��������� �� ��������� 2*/
		[TestMethod]
		void TestLog2()
		{
			Assert::AreEqual(3, log2(8)); // (��������������, ���������� �� ����)
		}
		
		/* ���� ������� ������*/
		[TestMethod]
		void TestNullTree()
		{
			Assert::AreEqual(0.0, sum_serial(NULL)); 
		}

		/* ���� ������ �� ������������� ����*/
		[TestMethod]
		void TestSingleTree()
		{
			tnode node = {5.0,0.0,NULL,NULL};
			Assert::AreEqual(0.0, sum_serial(&node)); 
		}

		/* ���� ����������������� �������� ����� ������*/
		[TestMethod]
		void TestSumSerial()
		{
			// ������ ������
			tnode* root = NULL;
			root = addNode(10,root);		// ������ 10
			addNode(5,root);				// ���� 5
			addNode(15,root);				// ���� 15
			addNode(7,root);				// ���� 7
			addNode(20.3,root);				// ���� 20.3
			double result = 5+15+7+20.3;	// ����� ��������
			Assert::AreEqual(result, sum_serial(root));
			freeTree(root);
		}

		/* ���� ������������� �������� ����� ������ (sections)*/
		[TestMethod]
		void TestSumParallel()
		{
			// ������ ������
			tnode* root = NULL;
			root = addNode(10,root);		// ������ 10
			addNode(5,root);				// ���� 5
			addNode(15,root);				// ���� 15
			addNode(7,root);				// ���� 7
			addNode(20.3,root);				// ���� 20.3
			double result = 5+15+7+20.3;	// ����� ��������
			Assert::AreEqual(result, sum_parallel_section(root));
			freeTree(root);
		}

		/* ���� ������������� �������� ����� ������ (pthread)*/
		[TestMethod]
		void TestSumParallelPthread()
		{
			// ������ ������
			tnode* root = NULL;
			root = addNode(10,root);		// ������ 10
			addNode(5,root);				// ���� 5
			addNode(15,root);				// ���� 15
			addNode(7,root);				// ���� 7
			addNode(20.3,root);				// ���� 20.3
			double result = 5+15+7+20.3;	// ����� ��������
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
