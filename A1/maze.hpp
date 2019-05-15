// Winter 2019

#pragma once
#include <stddef.h>
#include <memory.h>

class Maze
{
public:
	Maze( size_t dim );
	Maze(const Maze &m):m_values{new int[m.m_dim*m.m_dim]}, m_dim{m.m_dim} 
	{
		memcpy(m_values, m.m_values, m_dim*m_dim*sizeof(int));
	}
	~Maze();

	void reset();

	size_t getDim() const;

	int getValue( int x, int y ) const;

	void setValue( int x, int y, int h );

	void digMaze();
	void printMaze(); // for debugging
private:
	size_t m_dim;
	int *m_values;
	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
};
