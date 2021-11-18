#pragma once

#include "material.hpp"

#include <memory>

struct ShaderAssets
{
    std::shared_ptr<Shader> pbr = std::make_shared<Shader>("shaders/default.vert", "shaders/pbr.frag");
    std::shared_ptr<Shader> phong = std::make_shared<Shader>("shaders/default.vert", "shaders/phong.frag");
    std::shared_ptr<Shader> pbrTess = std::make_shared<Shader>("shaders/default_tess.vert", "shaders/tri_tess.tesc", "shaders/tri_tess.tese", "shaders/pbr.frag");
    std::shared_ptr<Shader> phongTess = std::make_shared<Shader>("shaders/default_tess.vert", "shaders/tri_tess.tesc", "shaders/tri_tess.tese", "shaders/phong.frag");
    std::shared_ptr<Shader> pbrTessQuad = std::make_shared<Shader>("shaders/default_tess.vert", "shaders/quad_tess.tesc", "shaders/quad_tess.tese", "shaders/pbr.frag");
    std::shared_ptr<Shader> phongTessQuad = std::make_shared<Shader>("shaders/default_tess.vert", "shaders/quad_tess.tesc", "shaders/quad_tess.tese", "shaders/phong.frag");
    std::shared_ptr<Shader> wavesPBR = std::make_shared<Shader>("shaders/waves.vert", "shaders/tri_tess.tesc", "shaders/tri_tess.tese", "shaders/pbr.frag");
    std::shared_ptr<Shader> wavesPhong = std::make_shared<Shader>("shaders/waves.vert", "shaders/tri_tess.tesc", "shaders/tri_tess.tese", "shaders/phong.frag");
    std::shared_ptr<Shader> wavesPBRQuad = std::make_shared<Shader>("shaders/waves.vert", "shaders/quad_tess.tesc", "shaders/quad_tess.tese", "shaders/pbr.frag");
    std::shared_ptr<Shader> wavesPhongQuad = std::make_shared<Shader>("shaders/waves.vert", "shaders/quad_tess.tesc", "shaders/quad_tess.tese", "shaders/phong.frag");

    std::shared_ptr<Shader> ppHDR = std::make_shared<Shader>("shaders/post_processing.vert", "shaders/hdr.frag");
    std::shared_ptr<Shader> ppACES = std::make_shared<Shader>("shaders/post_processing.vert", "shaders/aces.frag");
    std::shared_ptr<Shader> ppLinearToGamma = std::make_shared<Shader>("shaders/post_processing.vert", "shaders/linear_to_gamma.frag");
};

struct TextureAssets
{
    std::shared_ptr<Texture> boat = std::make_shared<Texture>("textures/boat.png");
    std::shared_ptr<Texture> beachBall = std::make_shared<Texture>("textures/beach_ball.png");
    std::shared_ptr<Texture> water = std::make_shared<Texture>("textures/water.png");
    std::shared_ptr<Texture> white = std::make_shared<Texture>("textures/white.png");
};

struct MaterialAssets
{
    MaterialAssets(std::shared_ptr<ShaderAssets> shaderAssets, std::shared_ptr<TextureAssets> textureAssets) : shaderAssets{ shaderAssets }, textureAssets{ textureAssets } {}

    std::shared_ptr<ShaderAssets> shaderAssets;
    std::shared_ptr<TextureAssets> textureAssets;

    std::shared_ptr<Material> pbrBoat = std::make_shared<Material>(textureAssets->boat, shaderAssets->pbrTess);
    std::shared_ptr<Material> phongBoat = std::make_shared<Material>(textureAssets->boat, shaderAssets->phongTess);
    std::shared_ptr<Material> pbrBoatQuad = std::make_shared<Material>(textureAssets->boat, shaderAssets->pbrTessQuad);
    std::shared_ptr<Material> phongBoatQuad = std::make_shared<Material>(textureAssets->boat, shaderAssets->phongTessQuad);

    std::shared_ptr<Material> pbrBall = std::make_shared<Material>(textureAssets->beachBall, shaderAssets->pbrTess);
    std::shared_ptr<Material> phongBall = std::make_shared<Material>(textureAssets->beachBall, shaderAssets->phongTess);
    std::shared_ptr<Material> pbrBallQuad = std::make_shared<Material>(textureAssets->beachBall, shaderAssets->pbrTessQuad);
    std::shared_ptr<Material> phongBallQuad = std::make_shared<Material>(textureAssets->beachBall, shaderAssets->phongTessQuad);

    std::shared_ptr<Material> pbrWaves = std::make_shared<Material>(textureAssets->water, shaderAssets->wavesPBR);
    std::shared_ptr<Material> phongWaves = std::make_shared<Material>(textureAssets->water, shaderAssets->wavesPhong);
    std::shared_ptr<Material> pbrWavesQuad = std::make_shared<Material>(textureAssets->water, shaderAssets->wavesPBRQuad);
    std::shared_ptr<Material> phongWavesQuad = std::make_shared<Material>(textureAssets->water, shaderAssets->wavesPhongQuad);

    std::shared_ptr<Material> pbrWhite = std::make_shared<Material>(textureAssets->white, shaderAssets->pbr);
};

struct ModelAssets
{
    std::shared_ptr<Model> hull = std::make_shared<Model>("models/hull.obj", false);
    std::shared_ptr<Model> hullQuads = std::make_shared<Model>("models/hull.obj", true);
    
    std::shared_ptr<Model> sail = std::make_shared<Model>("models/sail.obj", false);
    std::shared_ptr<Model> sailQuads = std::make_shared<Model>("models/sail.obj", true);

    std::shared_ptr<Model> sphere = std::make_shared<Model>("models/sphere.obj", false);
    std::shared_ptr<Model> sphereQuads = std::make_shared<Model>("models/sphere.obj", true);

    std::shared_ptr<Model> water = std::make_shared<Model>("models/water.obj", false);
    std::shared_ptr<Model> waterQuads = std::make_shared<Model>("models/water.obj", true);

    std::shared_ptr<Model> smallSphere = std::make_shared<Model>("models/small_sphere.obj", false);
};