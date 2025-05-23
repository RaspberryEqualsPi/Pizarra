//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Tile Map
//
// Copyright(c) 2016-2025 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SELBAWARD_TILEMAP_HPP
#define SELBAWARD_TILEMAP_HPP

#include "Common.hpp"

#include <deque>
#include <array>
#include <cmath>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace selbaward
{

// SW Tile Map v2.0.4
template <class T>
class TileMap : public sf::Drawable, public sf::Transformable
{
public:
	TileMap();
	void update();
	void setLevel();
	void setLevel(const std::vector<T>& level);
	void setLevel(const std::deque<T>& level);
	void setLevel(const T* level, std::size_t size);
	template <class U>
	void setLevel(const U& level, std::size_t width);
	void setLevel(const T* level, std::size_t size, std::size_t width);
	void setLevelWidth(std::size_t width);
	std::size_t getLevelWidth() const;
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;
	void setGridSize(sf::Vector2<std::size_t> gridSize);
	sf::Vector2<std::size_t> getGridSize() const;
	std::size_t getTotalGridSize() const;
	void setOutOfBoundsTile(std::size_t textureTileIndex);
	void setTexture(const sf::Texture& texture);
	void setTexture();
	void setNumberOfTextureTilesPerRow(std::size_t numberOfTextureTilesPerRow);
	void setTextureOffset(sf::Vector2<std::size_t> textureOffset);
	void setTextureTileSize(sf::Vector2<std::size_t> textureTileSize);
	sf::Vector2<std::size_t> getTextureTileSize() const;
	void setSmooth(bool smooth);
	bool getSmooth() const;
	void setSmoothScroll(bool smoothScroll);
	bool getSmoothScroll() const;
	void setCameraTargetTile(sf::Vector2f cameraTargetTile);
	sf::Vector2f getCameraTargetTile() const;
	void setCamera(sf::Vector2f camera);
	sf::Vector2f getCamera() const;
	void setColor(sf::Color color);
	sf::Color getColor() const;
	sf::Vector2i getLevelPositionAtCoord(sf::Vector2f coord) const;
	std::size_t getTileAtCoord(sf::Vector2f coord) const;
	sf::Vector2f getCoordAtLevelGridPosition(sf::Vector2f levelGridPosition) const; // takes a float vector so the parameter can specify different parts of that tile e.g. (2.5, 1.5) = centre of tile (2, 1)
	sf::Vector2f getTileSize() const; // display size of a tile before any transformations

	void redraw();









private:
	struct ActionFlags
	{
		bool scrollSmoothly{ false };
	};

	struct StateFlags
	{
		bool smooth{ false };
	};

	// flags
	ActionFlags m_do;
	StateFlags m_is;

	// external level data
	enum class LevelContainerType
	{
		None,
		Vector,
		Deque,
		Raw
	} m_levelContainerType;
	std::size_t m_levelWidth;
	std::size_t m_levelSize;
	const void* m_pLevel;

	// data
	sf::Vector2<std::size_t> m_gridSize;
	std::vector<unsigned long int> m_grid;
	std::size_t m_outOfBoundsTile;

	// camera (in tiles)
	sf::Vector2f m_camera;
	sf::Vector2f m_cameraTarget;

	// color
	sf::Color m_color;

	// tiles
	sf::Vector2f m_size;
	const sf::Texture* m_texture;
	std::size_t m_numberOfTextureTilesPerRow;
	sf::Vector2<std::size_t> m_textureOffset;
	sf::Vector2<std::size_t> m_textureTileSize;
	mutable std::vector<sf::Vertex> m_vertices;

	// render
	mutable bool m_redrawRequired;
	mutable sf::RenderTexture m_renderTexture;
	mutable std::vector<sf::Vertex> m_render;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void priv_updateVertices() const;
	void priv_updateRender() const;
	void priv_recreateRenderTexture();
	sf::Vector2i priv_getGridPositionAtCoord(sf::Vector2f coord) const;
	std::size_t priv_getTileAtGridPosition(sf::Vector2i gridPosition) const;
	sf::Vector2f priv_getActualCamera() const;
	sf::Vector2f priv_getTileOffsetFromVector(sf::Vector2f vector) const;
	sf::Vector2f priv_getVectorFromTileOffset(sf::Vector2f offset) const;
	template <class U>
	U priv_getTotalSizeFromSizeVector(sf::Vector2<U> vector) const;
};

} // namespace selbaward
#include "TileMap.inl"
#endif // SELBAWARD_TILEMAP_HPP
