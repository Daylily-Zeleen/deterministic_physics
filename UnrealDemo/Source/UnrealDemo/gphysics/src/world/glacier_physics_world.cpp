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
#include "glacier_physics_world.h"
#include "glacier_vector.h"
#include "glacier_transform_qt.h"
#include "glacier_collision_shape.h"
#include "glacier_physics_utils.h"
#include "glacier_debug_draw.h"
#include "glacier_collision_object.h"
#include "glacier_rigid_dynamic.h"

void GGridCell::DebugDraw(IGlacierDraw* pDraw) const
{
    pDraw->DrawBox( GTransform_QT::Identity(), GetCenter(), GetHalfSize(),0x00FFFF00 );

    for (int32_t i = 0; i < (int32_t)m_Objects.size(); ++i)
    {
        const GCollisionObject* pObject = m_Objects[i];
        GPhyscsUtils::DrawShape( pObject->m_Transform, pObject->m_Shape, pDraw);
    }

}

bool GGridCell::AddCollisionObject(GCollisionObject* pObject)
{
    if (std::find(m_Objects.begin(), m_Objects.end(), pObject) != m_Objects.end())
    {
        return false;
    }


    m_Objects.push_back(pObject);

    return true;
}


bool GPhysicsWorld::AddCollisionObject(GCollisionObject* pObject)
{
    if (std::find(m_Objects.begin(), m_Objects.end(), pObject) != m_Objects.end())
    {
        return false;
    }

    m_Objects.push_back(pObject);


    GVector3 VPos = pObject->m_Transform.m_Translation;

    GGridPosition CellPos;

    CellPos.x = GMath::FloorToInt(VPos.x / m_nCellWide );
    CellPos.y = GMath::FloorToInt(VPos.y / m_nCellWide);
    CellPos.z = GMath::FloorToInt(VPos.z / m_nCellHeight);

    GGridCell* pCell = nullptr;

    std::map<GGridPosition, GGridCell*>::iterator iter = m_Grids.find(CellPos); 

    if( iter != m_Grids.end() )
    {
        pCell = iter->second;
    }
    else
    {
        pCell = new GGridCell( CellPos, m_nCellWide, m_nCellHeight );
        m_Grids[CellPos] = pCell;
    }

    if( pCell != nullptr)
    {
        pCell->AddCollisionObject(pObject);
    }

    return true;
}

void GPhysicsWorld::PreTick()
{

}

GGridPosition s_BroadPhaseNeighbourt[13] =
{
    GGridPosition( 1, -1, -1 ),
    GGridPosition( 1,  0, -1 ),
    GGridPosition( 1,  1, -1 ),
    GGridPosition( 1, -1,  0 ),
    GGridPosition( 1,  0,  0 ),
    GGridPosition( 1,  1,  0 ),
    GGridPosition( 1, -1,  1 ),
    GGridPosition( 1,  0,  1 ),
    GGridPosition( 1,  1,  1 ),
    GGridPosition( 0,  1, -1 ),
    GGridPosition( 0,  1,  0 ),
    GGridPosition( 0,  1,  1 ),
    GGridPosition( 0,  0,  1 )
};


void GPhysicsWorld::SimulateTick(f32 DetltaTime)
{
    for (int32_t i = 0; i < (int32_t)m_Objects.size(); ++i)
    {
        GCollisionObject* pObject = m_Objects[i];
        if( pObject->m_CollisionType == ECollisionObjectType::Dynamic)
        {
            GDynamicRigid* pDynamicRigid = (GDynamicRigid*)pObject;
            pDynamicRigid->Tick_PreTransform( DetltaTime );
        }
    }

    // broadphase
    m_BroadPhasePairs.clear();
    for (std::map<GGridPosition, GGridCell*>::const_iterator iter = m_Grids.begin(); iter != m_Grids.end(); ++iter)
    {
        // ceil 
        for( int32_t i = 0; i < (int32_t)iter->second->m_Objects.size(); ++i )
        {
            GCollisionObject* pObjectA = iter->second->m_Objects[i];

            if( pObjectA->GetCollisionObjectType() != ECollisionObjectType::Dynamic)
            {
                continue;
            }
            const GAABB& BoxA =  pObjectA->GetAABB();
            for (int32_t j = 0; j < i; ++i)
            {
                GCollisionObject* pObjectB = iter->second->m_Objects[j]; 
                const GAABB& BoxB =  pObjectB->GetAABB();
                if( BoxA.Intersects(BoxB) )
                {
                    m_BroadPhasePairs.push_back( GBroadPhasePair( pObjectA, pObjectB));       
                }
            }
        }
         // other ceil

        for (int32_t i = 0; i < 13; ++i)
        {
            GGridPosition TPos = s_BroadPhaseNeighbourt[i] + iter->first;

            std::map<GGridPosition, GGridCell*>::const_iterator iterB = m_Grids.find( TPos );

            if( iterB == m_Grids.end() )
                continue;

            for (int32_t LoopA = 0; i < (int32_t)iter->second->m_Objects.size(); ++LoopA)
            {
                GCollisionObject* pObjectA = iter->second->m_Objects[LoopA];

                if (pObjectA->GetCollisionObjectType() != ECollisionObjectType::Dynamic)
                {
                    continue;
                }

                const GAABB& BoxA =  pObjectA->GetAABB();
                for (int32_t LoopB = 0; i < (int32_t)iterB->second->m_Objects.size(); ++LoopB)
                {
                    GCollisionObject* pObjectB = iter->second->m_Objects[LoopA];

                    if( pObjectA->GetId() <  pObjectB->GetId())
                    {
                        const GAABB& BoxB = pObjectB->GetAABB();
                        if (BoxA.Intersects(BoxB))
                        {
                            m_BroadPhasePairs.push_back(GBroadPhasePair(pObjectA, pObjectB));
                        }
                    }
                }
            }
        }
    }

    // 

}

void GPhysicsWorld::PostTick()
{


}


void GPhysicsWorld::DebugDraw(IGlacierDraw* pDraw ) const
{
   // std::map<GGridPosition, GGridCell*> m_Grids;

    for( std::map<GGridPosition, GGridCell*>::const_iterator iter = m_Grids.begin(); iter != m_Grids.end(); ++iter )
    {
        iter->second->DebugDraw(pDraw);
    }

}