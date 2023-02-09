#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "textureUtil.h"
#include <cstdio>

bool TEXTURE_DGB = false;

Texture2D::Texture2D(const char *texturePath)
{
    GLint width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // set wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image data
    unsigned char *image_data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    if (TEXTURE_DGB)
    {
        printf("Image width: %d\n", width);
        printf("Image height: %d\n", height);
        printf("Image channels: %d\n", nrChannels);
        printf("%d %d %d \n", image_data[0], image_data[1], image_data[2]);
    }

    if (image_data)
    {
        // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (nrChannels == 4)
        {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,       // mipmap level
                GL_RGBA, // storage type of image
                width,
                height,
                0,       // legacy styff
                GL_RGBA, // type of source image
                GL_UNSIGNED_BYTE,
                image_data);
        }
        else
        {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,      // mipmap level
                GL_RGB, // storage type of image
                width,
                height,
                0,      // legacy styff
                GL_RGB, // type of source image
                GL_UNSIGNED_BYTE,
                image_data);
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image_data);
    }
    else
    {
        printf("Issue loading image data\n");
    }
}

GLuint Texture2D::getTextureID() const
{
    return textureID;
}

TextureCube::TextureCube(std::vector<std::string> faces)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

GLuint TextureCube::getTextureID() const
{
    return textureID;
}