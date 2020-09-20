#include <cstdarg>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <renderer/Mesh.hpp>
#include <renderer/Texture.hpp>

/**
 * CONSTRUCTOR
 */
Mesh::Mesh(std::vector<float> &vertices, std::vector<unsigned int> &indices,
             std::ifstream const &vertShaderFile, std::ifstream const &fragShaderFile,
             std::vector<std::string> const &textureFilepaths)
    : shader(vertShaderFile, fragShaderFile),
      texture(textureFilepaths),
      numTriangles(indices.size() / 3) {

  // Create vertex array object
  glGenVertexArrays(1, &(this->vertArrObj));
  // Create vertex buffer object
  glGenBuffers(1, &(this->vertBufObj));
  // Create element buffer object
  glGenBuffers(1, &(this->elementBufObj));

  // Bind vertex array
  glBindVertexArray(this->vertArrObj);

  // Copy vertex array into vertex buffer memory
  glBindBuffer(GL_ARRAY_BUFFER, this->vertBufObj);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // Copy vertex array into element buffer memory
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBufObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  // Define vertex data interpretation
  glVertexAttribPointer(
    0 /* Location of vertex attribute in shader */,
    3 /* Number of components in each attribute */,
    GL_FLOAT /* Data type of each component */,
    GL_FALSE /* Whether data should be normalized */,
    8 * sizeof(float) /* Byte offset between consecutive attributes */,
    (void *) 0 /* Offset of first component of first attribute in array */
  );
  glEnableVertexAttribArray(0);

  // Define vertex data interpretation
  glVertexAttribPointer(
    1 /* Location of vertex attribute in shader */,
    3 /* Number of components in each attribute */,
    GL_FLOAT /* Data type of each component */,
    GL_FALSE /* Whether data should be normalized */,
    8 * sizeof(float) /* Byte offset between consecutive attributes */,
    (void *)(3 * sizeof(float)) /* Offset of first component of first attribute in array */
  );
  glEnableVertexAttribArray(1);

  // Define vertex data interpretation
  glVertexAttribPointer(
    2 /* Location of vertex attribute in shader */,
    2 /* Number of components in each attribute */,
    GL_FLOAT /* Data type of each component */,
    GL_FALSE /* Whether data should be normalized */,
    8 * sizeof(float) /* Byte offset between consecutive attributes */,
    (void *)(6 * sizeof(float)) /* Offset of first component of first attribute in array */
  );
  glEnableVertexAttribArray(2);
}

/**
 * PUBLIC METHODS
 */

void Mesh::preDraw() {
  glUseProgram(this->shader.getShaderProgramObjId());
  for (size_t i = 0; i < this->texture.getNumTextures(); ++i) {
    std::string const texName = "tex" + std::to_string(i);
    this->shader.setUniform(texName, (int) i /* TODO: don't cast */);
  }
}

void Mesh::draw() {
  glUseProgram(this->shader.getShaderProgramObjId());
  configureShader();
  configureTexture();
  for (size_t i = 0; i < this->texture.getNumTextures(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, this->texture.getTextureObjId(i));
  }
  glBindVertexArray(this->vertArrObj);
  glDrawElements(GL_TRIANGLES, this->numTriangles * 3, GL_UNSIGNED_INT, 0);
}
