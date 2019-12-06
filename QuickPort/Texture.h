#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
	public:
		unsigned int	*texture;			// OpenGL texture name
		int				num_frames;			// number of frames in the texture
		int				w, h;				// width and height
		float			w_tex_dist;			// length in feet that the texture will cover in the world, this is based on Options.TEXSIZE representing 10 feet
		float			h_tex_dist;			// same, but for the height value of the texture
		char			*filename;			// filename of the texture

		void LoadTexture(const char *_filename,const int numframes,const bool clamp);
		Texture();
		~Texture();
};

#endif