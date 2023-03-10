#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <d3d11.h>

#include "D3DUtil.h"

//handy rectangle definer
struct RECTF
{
	float left, top, right, bottom;  //coordinates for part of a sprite (or anything else rectangular)
	operator RECT() {
		return RECT{ (int)left,(int)top,(int)right,(int)bottom };
	}
}; 

//we only ever want one unique texture to be loaded
//it can then be shared between any meshes that need it
class TexCache
{
public:
	//associate a file name with a texture resource
	struct Data
	{
		//constructors
		Data() {}
		Data(const std::string& fName, ID3D11ShaderResourceView*p, const DirectX::SimpleMath::Vector2& _dim)
			: fileName(fName), pTex(p), dim(_dim)
		{
			frames.clear();
		}
		Data(const std::string& fName, ID3D11ShaderResourceView*p, const DirectX::SimpleMath::Vector2& _dim, const std::vector<RECTF> *_frames)
			:fileName(fName), pTex(p), dim(_dim)

		{
			if (_frames)
				frames = *_frames;
		}

		std::string fileName;						//texture file name
		ID3D11ShaderResourceView* pTex = nullptr;	//pointer to d3d texture object
		DirectX::SimpleMath::Vector2 dim;			//width and height in texels
		std::vector<RECTF> frames;					//optional array of sub-ractangles within the texture in texels
	};

	//tidy up at the end
	void Release();
	//if this texture is new load it in, otherwise find it and return a handle
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device*pDevice, const std::string& fileName, const std::string& texName="", bool appendPath=true, const std::vector<RECTF> *_frames = nullptr);
	//usually we just have a texture file name, but they're all in a sub folder
	void SetAssetPath(const std::string& path) {
		mAssetPath = path;
	}
	//where are the textures?
	const std::string& GetAssetPath() const { return mAssetPath; }
	//pull out a texture by nickname = fast
	//for top speed the texture key would be a unique integer ID, but names are easier
	Data& Get(const std::string& texName) {
		return mCache.at(texName);
	}
	//slowly find a texture by handle
	const Data& Get(ID3D11ShaderResourceView *pTex);

private:
	/*
	* find the texture this d3d handle points at, return its size
	* pTex - IN texture we are interested in
	* returns - width and height of texture in texels
	*/
	DirectX::SimpleMath::Vector2 GetDimensions(ID3D11ShaderResourceView* pTex);
	//array of texture data
	typedef std::unordered_map<std::string, Data> MyMap;
	MyMap mCache;

	//some data sub folder with all the textures in e.g. /data/textures
	//useful if you just want to specify textures by file name in the code
	//and not write the path everywhere
	std::string mAssetPath;
};


