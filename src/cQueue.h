#pragma once

template <class TItem>
class cQueue
{
private:
	TItem *mData;
	TItem *mHead;
	TItem *mTail;

public:
    cQueue() {  };
	cQueue(uint length);
	~cQueue();

	inline void Reset();
	inline void Put(const TItem &item);
    inline bool IsEmpty();
	inline TItem* Get();
};

template <class TItem>
cQueue<TItem>::cQueue(uint length)
{
	mData = new TItem[length];
	mHead = mData;
	mTail = mData;
}

template <class TItem>
cQueue<TItem>::~cQueue()
{
	/*if (mData != NULL)
	{
		delete mData;
		mData = mHead = mTail = NULL;
	}*/
}

template <class TItem>
inline void cQueue<TItem>::Reset()
{
	mHead = mData;
	mTail = mData;
}

template <class TItem>
inline bool cQueue<TItem>::IsEmpty()
{
	return this->mHead == this->mTail;
}

template <class TItem>
inline void cQueue<TItem>::Put(const TItem& item)
{
	*mTail++ = item;
}

template <class TItem>
inline TItem* cQueue<TItem>::Get()
{
	TItem *item = NULL;

	if (mHead < mTail)
	{
		item = mHead;
		mHead++;
	}
	return item;
}