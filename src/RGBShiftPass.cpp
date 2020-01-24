/*
 *  RGBShiftPass.h
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
#include "RGBShiftPass.h"
#include "ofMain.h"

namespace itg
{
    RGBShiftPass::RGBShiftPass(const ofVec2f& aspect, bool arb, float amount, float angle) :
        amount(amount), angle(angle), RenderPass(aspect, arb, "RGBShift")
    {
        string vertShaderSrc = STRINGIFY(
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
                                         uniform sampler2D tDiffuse;
                                         uniform float amount;
                                         uniform float angle;

                                         in vec2 vTexCoord;
                                         out vec4 fragColor;
                        
                                         void main() {
                                         vec2 vUv = vTexCoord;
                                         vec2 offset = amount * vec2( cos(angle), sin(angle));
                                         vec4 cr = texture(tDiffuse, vUv + offset);
                                         vec4 cga = texture(tDiffuse, vUv);
                                         vec4 cb = texture(tDiffuse, vUv - offset);
                                         fragColor = vec4(cr.r, cga.g, cb.b, cga.a);
                                         
                                         }
        );
        
        ostringstream oss;
        oss << "#version 150" << endl;
        oss << vertShaderSrc << endl;
        shader.setupShaderFromSource(GL_VERTEX_SHADER, oss.str());
        
        oss.str("");
        oss << "#version 150" << endl;
        oss << fragShaderSrc << endl;
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());
        
		shader.bindDefaults();
        shader.linkProgram();
        
    }
    

    void RGBShiftPass::render(ofFbo& readFbo, ofFbo& writeFbo, ofTexture& depthTex)
    {
        writeFbo.begin();
        
        
        shader.begin();
        
        shader.setUniformTexture("tDiffuse", readFbo.getTexture(), 0);
        shader.setUniform1f("amount", amount);
        shader.setUniform1f("angle", angle);
        
        //texturedQuad(0, 0, writeFbo.getWidth(), writeFbo.getHeight());
		quad.draw(OF_MESH_FILL);
        
        shader.end();
        writeFbo.end();
    }
}
