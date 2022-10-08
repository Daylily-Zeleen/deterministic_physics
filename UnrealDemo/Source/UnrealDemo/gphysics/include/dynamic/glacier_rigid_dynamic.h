/* 
 * Copyright (C) 2022 zhou xuan, Email: zhouxuan6676@gmail.com
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at * 
 * http://www.apache.org/licenses/LICENSE-2.0 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */
#pragma once

#include "glacier_rigid_body.h"

class GDynamicRigid : public GRigidBody
{
public:



    void Tick_PreTransform( const f32 DetalTime )
    {
        m_Transform_Pre.m_Translation += m_Velocity * DetalTime;
        //m_Transform_Pre.
    
    }



public:

    GTransform_QT   m_Transform_Pre;

    f32             m_Mass;
    f32             m_InvMass;
    GVector3        m_Velocity;

    GVector3        m_MoumentInertia;
    GVector3        m_InvMoumentInertia;
    GVector3        m_AngleVelocity;





};