#ifndef RESULT_RENDERER_H
#define RESULT_RENDERER_H

#include "types.h"
#include <memory>
#include <string>

class Renderer
{
public:
    virtual void Render(const PlayersTable& table) = 0;
};

typedef std::unique_ptr<Renderer> RendererPtr;

RendererPtr getConsoleRenderer();
RendererPtr getFileRenderer(const std::string& filename);

#endif //RESULT_RENDERER_H