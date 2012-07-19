template <typename T>
class Array3D
{
public:
	Array3D(unsigned xDim, unsigned, yDim, unsigned zDim) : nXDim(xDim), nYDim(yDim), nZDim(zDim), pArray(0)
	{
		if(xDim > 0 && yDim > 0 && zDim >0)
			pArray = new T[xDim * yDim * zDim];
	}

	~Array3D()
	{
		delete[] pArray;
	}

	const T& operator () (unsigned x, unsigned y, unsigned z)
	{
		return pArray [z*nYDim*nXDim + y*nXDim + x];
	}

	T& operator () (unsigned x, unsigned y, unsigned z)
	{
		return pArray [z*nYDim*nXDim + y*nXDim + x];
	}

	unsigned GetXDim() const { return nXDim; }
	unsigned GetYDim() const { return nYDim; }
	unsigned GetZDim() const { return nZDim; }

private:
	unsigned nXDim;
	unsigned nYDim;
	unsigned nZDim;

	T* pArray;

	Array3D(const Array3D<T>&);
	Array3D& operator = (const Array3D<T>&);	
}