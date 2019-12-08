/*
 *  RimHighlightingPass.cpp
 *
 *  Copyright (c) 2013, satcy, http://satcy.net
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "RimHighlightingPass.h"

namespace itg
{
    RimHighlightingPass::RimHighlightingPass(const ofVec2f& aspect, bool arb) :
        RenderPass(aspect, arb, "rimhighlighting")
    {
        string vertShaderSrc = STRINGIFY(
                                         #version 410 core\n
                                         uniform mat4 modelViewProjectionMatrix; 

                                         in vec2 texcoord;
                                         in vec3 normal;
                                         in vec4 position;
                                         in vec4 color;

                                         out vec3 vNormal;
                                         out vec3 sides;
                                         out vec2 vTexCoord;
                                         out vec4 vColor;
                                         
                                         void main()
        {
            vNormal = gl_NormalMatrix * normal;
            gl_Position = position;
            sides = gl_Position.xyz;
            sides.x = sides.x - 128.0;
            
            // vTexCoord = vec2(gl_MultiTexCoord0);
            vTexCoord = texcoord;
            // v_color = gl_Color;
            vColor = color;
        }
                                         );
        
        string fragShaderSrc = STRINGIFY(
                                         #version 410 core\n
                                         in vec3 vNormal;
                                         in vec3 sides;
                                         in vec2 vTexCoord;
                                         uniform sampler2D myTexture;
                                         in vec4 vColor;

                                         out vec4 fragColor;
                                         
                                         void main()
        {
            float intensity;
            vec3 n = normalize(vNormal);
            vec4 color;
            intensity = dot(sides,n);
            
            fragColor = texture(myTexture, vTexCoord);
            if (intensity >= 64.0)
            {
                fragColor.b = fragColor.b / 1.5;
                fragColor.r = fragColor.r * 1.5;
                fragColor.g = fragColor.g * 1.25;
            }
            else
            {
                fragColor = fragColor * vColor;
            }
        }
        );
        
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
		shader.bindDefaults();
        shader.linkProgram();

    }
    
    void RimHighlightingPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        shader.begin();
        shader.setUniformTexture("myTexture", readFbo.getTexture(), 0);
        
        //texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);
        
        shader.end();
        writeFbo.end();
    }
}
