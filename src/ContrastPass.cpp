/*
 *  ContrastPass.h
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
#include "ContrastPass.h"
#include "ofMain.h"

namespace itg
{
    ContrastPass::ContrastPass(const ofVec2f& aspect, bool arb, float contrast, float brightness) :
        contrast(contrast), brightness(brightness), RenderPass(aspect, arb, "contrast")
    {
        multiple = 1.0f;

        string vertShaderSrc = STRINGIFY(
                                         #version 150\n
                                         in vec2 texcoord;
                                         in vec4 position;
                                         uniform mat4 modelViewProjectionMatrix;
                                         out vec2 vTexCoord;
                                         void main() {
											 gl_Position = position;
                                             vTexCoord = texcoord;
                                         }
        );

        string fragShaderSrc = STRINGIFY(
                                         #version 150\n
                                         uniform sampler2D tex0;
                                         uniform float contrast;
                                         uniform float brightness;
                                         uniform float multiple;

                                         in vec2 vTexCoord;

                                         out vec4 fragColor;
                                         
                                         void main(){
                                             vec4 color = texture(tex0,vTexCoord);
                                             
                                             float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
                                             p = p * brightness;
                                             vec4 color2 = vec4(p,p,p,1.0);
                                             color *= color2;
                                             color *= vec4(multiple,multiple,multiple,1.0);
                                             color = mix( vec4(1.0,1.0,1.0,1.0),color,contrast);
                                             
                                             fragColor = vec4(color.r , color.g, color.b, 1.0);
                                         }
        );
        
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertShaderSrc);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderSrc);
		shader.bindDefaults();
        shader.linkProgram();
        
    }
    

    void ContrastPass::render(ofFbo& readFbo, ofFbo& writeFbo)
    {
        writeFbo.begin();
        
        ofClear(0, 0, 0, 255);
        
        shader.begin();
        
        shader.setUniformTexture("tex0", readFbo, 0);
        shader.setUniform1f("contrast", contrast);
        shader.setUniform1f("brightness", brightness);
        shader.setUniform1f("multiple", multiple);
        
        //texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);

        shader.end();
        writeFbo.end();
    }
}