#ifndef LEVEL_H_
#define LEVEL_H_

#include "GameConstants.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

class Level
{
public:

	enum MazeEntry {
		empty, player, left_kong, right_kong, floor, ladder, bonfire,
		fireball, koopa, extra_life, garlic
	};
	enum LoadResult {
		load_success, load_fail_file_not_found, load_fail_bad_format};

	Level(std::string assetPath)
	 : m_pathPrefix(assetPath)
	{
		for (int y = 0; y < VIEW_HEIGHT; y++)
			for (int x = 0; x < VIEW_WIDTH; x++)
				m_maze[y][x] = empty;

		if (!m_pathPrefix.empty())
			m_pathPrefix += '/';
	}

	LoadResult loadLevel(std::string filename)
	{
		std::ifstream levelFile((m_pathPrefix + filename).c_str());
		if (!levelFile)
			return load_fail_file_not_found;

		  // get the maze

		std::string line;
		int numPlayers = 0;
		int numKongs = 0;

		for (int y = VIEW_HEIGHT-1; std::getline(levelFile, line); y--)
		{
			if (y < 0)	// too many maze lines?
			{
				if (line.find_first_not_of(" \t\r") != std::string::npos)
					return load_fail_bad_format;  // non-blank line
				char dummy;
				if (levelFile >> dummy)	 // non-blank rest of file
				return load_fail_bad_format;
				break;
			}

			if (line.size() < VIEW_WIDTH  ||  line.find_first_not_of(" \t\r", VIEW_WIDTH) != std::string::npos)
				return load_fail_bad_format;
				
			for (int x = 0; x < VIEW_WIDTH; x++)
			{
				MazeEntry me;
				switch (toupper(line[x]))
				{
					default:   return load_fail_bad_format;
					case ' ':  me = empty; break;
					case 'P':  me = player; numPlayers++; break;
					case '<':  me = left_kong; numKongs++; break;
					case '>':  me = right_kong; numKongs++; break;
					case '@':  me = floor; break;
					case '#':  me = ladder; break;
					case 'B':  me = bonfire; break;
					case 'F':  me = fireball; break;
					case 'K':  me = koopa; break;
					case 'E':  me = extra_life; break;
					case 'G':  me = garlic; break;
				}
				m_maze[y][x] = me;
			}
		}

		if (numPlayers != 1  ||  numKongs != 1)
			return load_fail_bad_format;

		return load_success;
	}

	MazeEntry getContentsOf(int x, int y) const
	{
		if (x < 0  ||  x >= VIEW_WIDTH  ||  y < 0  ||  y >= VIEW_HEIGHT)
			return empty;
		return m_maze[y][x];
	}

private:

	MazeEntry	m_maze[VIEW_HEIGHT][VIEW_WIDTH];
	std::string m_pathPrefix;

	bool edgesValid() const
	{
		for (int y = 0; y < VIEW_HEIGHT; y++)
			if (m_maze[y][0] != floor || m_maze[y][VIEW_WIDTH-1] != floor)
				return false;
		for (int x = 0; x < VIEW_WIDTH; x++)
			if (m_maze[0][x] != floor ||
					(m_maze[VIEW_HEIGHT-1][x] != floor && m_maze[VIEW_HEIGHT-1][x] != ladder))
				return false;

		return true;
	}
};

#endif // LEVEL_H_
