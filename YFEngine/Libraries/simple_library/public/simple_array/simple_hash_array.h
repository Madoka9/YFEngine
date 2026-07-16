// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "simple_library/public/simple_core_minimal/simple_c_core/simple_core_minimal.h"
#include <functional>

template<typename T>
class TIndexedContainerIterator
{
public:
	//构造
	TIndexedContainerIterator(T* InIter, int InN)
		:Iter(InIter)
	{
		Iter = InIter + InN;
	}

	//	for (; It != AAA.End();) {}
	bool operator!=(const TIndexedContainerIterator& InIterator)
	{
		return Iter != InIterator.Iter;
	}

	T& operator++(int)
	{
		++Iter;
		return *this;
	}

	T& operator--(int)
	{
		--Iter;
		return *this;
	}

	T& operator*()
	{
		return *Iter;
	}

protected:
	T* Iter;
};

template<typename ElementType>
class TArray
{
public:
	typedef TIndexedContainerIterator<ElementType> TIterator;

	TArray()
		:Data(nullptr)
		, NestIndex(0)
		, PreAllocationSize(10)
	{
		//首先预分配10个元素
		Data = (unsigned char*)malloc(PreAllocationSize * sizeof(ElementType));
	}

	~TArray()
	{
		free(Data);
	}

	int Num()
	{
		return NestIndex;
	}

	int AddUninitialized(int InLength)
	{
		if (InLength >= PreAllocationSize)
		{
			PreAllocationSize = InLength;
			Data = (ElementType*)realloc(Data, InLength * sizeof(ElementType));
		}

		memset(&Data[NestIndex], 0, sizeof(ElementType));

		int LastPos = NestIndex;
		NestIndex += InLength;

		return LastPos;
	}

	void Add(ElementType&& InType)
	{
		Add(InType);
	}

	void Add(ElementType* InType)
	{
		if (NestIndex >= PreAllocationSize)
		{
			Data = (ElementType*)realloc(Data, sizeof(ElementType) * (NestIndex + 10));
		}

		Data[NestIndex] = InType;
	}

	void Add(ElementType& InType)
	{
		int ElementSize = sizeof(ElementType);

		if (NestIndex >= PreAllocationSize)
		{
			//内存不够了 不够就分配		
			Data = (unsigned char*)realloc(Data, ElementSize * (NestIndex + 10));
		}

		memset(&Data[NestIndex * ElementSize], 0, ElementSize);
		memcpy(&Data[NestIndex * ElementSize], &InType, ElementSize);

		NestIndex++;
	}

	ElementType* Find(std::function<bool(ElementType&)> Func)
	{
		int InElementSize = sizeof(ElementType);
		int CurrentIndex = 0;
		unsigned char* Ptr = (unsigned char*)Data;
		while (CurrentIndex < NestIndex)
		{
			if (ElementType* InType = (ElementType*)Ptr)
			{
				if (Func(*InType))
				{
					return InType;
				}
			}

			Ptr += (unsigned char)InElementSize;
			CurrentIndex++;
		}

		return nullptr;
	}

	int Find(const ElementType& InElement)
	{
		int CurrentIndex = 0;
		int InElementSize = sizeof(ElementType);

		unsigned char* Ptr = Data;

		while (CurrentIndex < NestIndex)
		{
			if (ElementType* InType = (ElementType*)Ptr)
			{
				if (*InType == InElement)
				{
					return CurrentIndex;
				}
			}

			Ptr += (unsigned char)InElementSize;
			CurrentIndex++;
		}

		return -1;
	}

	bool IsValid(int InIndex)
	{
		return (NestIndex - 1) >= InIndex && InIndex >= 0;
	}

	bool RemoveAt(const int InIndex)
	{
		if (IsValid(InIndex))
		{
			int CurrentIndex = NestIndex - 1;

			if (CurrentIndex != -1)
			{
				int CpyNum = CurrentIndex - InIndex;

				int ElementSize = sizeof(ElementType);
				if (CpyNum > 0)//代表移除的是中间的
				{
					memcpy(&Data[InIndex * ElementSize], &Data[(InIndex + 1) * ElementSize], CpyNum * ElementSize);

					NestIndex--;

					return true;
				}
				else if (CpyNum == 0)//移除的对象就是最后一个
				{
					NestIndex--;

					return true;
				}
				else
				{
					//check(0);//不应该走这边
				}
			}
		}

		return false;
	}

	bool RemoveAt(const ElementType& InElement)
	{
		int CurrentIndex = NestIndex - 1;

		int Pos = Find(InElement);
		if (Pos != -1)
		{
			return RemoveAt(Pos);
		}

		return false;
	}

	ElementType& operator[](int Index)
	{
		return Data[Index];
	}

	TIterator Begin()
	{
		return TIterator(Data, 0);
	}

	TIterator End()
	{
		return TIterator(Data, NestIndex);
	}

	unsigned char* GetData()
	{
		return Data;
	}

protected:
	unsigned char* Data;//连续内存
	int NestIndex;//当前的index
	int PreAllocationSize;//预分配的数量
};