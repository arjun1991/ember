/***************************************************************************
PagingLandScapeOctreescenemanager.h  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net
***************************************************************************/

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#ifndef PagingLandScapeOctreeSCENEMANAGER_H
#define PagingLandScapeOctreeSCENEMANAGER_H

#include "OgreSceneManager.h"
#include "OgreRenderOperation.h"
#include "OgreSphere.h"

#include <list>
#include <algorithm>

#include "OgrePagingLandScapePrerequisites.h"

#include "OgrePagingLandScapeOctree.h"
#include "OgrePagingLandScapeOcclusion.h"
#include "OgreRenderable.h"

#include "OgrePagingLandScapePoolSet.h"

namespace Ogre
{

    /// Factory for OctreeSceneManager
    class PagingLandScapeOctreeSceneManagerFactory : public SceneManagerFactory
    {
    protected:
        void initMetaData(void) const;
    public:
        PagingLandScapeOctreeSceneManagerFactory() {}
        ~PagingLandScapeOctreeSceneManagerFactory() {}
        /// Factory type name
        static const String FACTORY_TYPE_NAME;
        SceneManager* createInstance(const String& instanceName);
        void destroyInstance(SceneManager* instance);
    };

    class PagingLandScapeOctreeNode;

    class PagingLandScapeOctreeCamera;
    class PagingLandScapeOctreeIntersectionSceneQuery;
    class PagingLandScapeOctreeRaySceneQuery;
    class PagingLandScapeOctreeSphereSceneQuery;
    class PagingLandScapeOctreeAxisAlignedBoxSceneQuery;
    class PagingLandScapeOctreePlaneBoundedVolumeListSceneQuery;

    /** A collection of pre-allocated octrees
    */
    class OctreeSet : public PoolSet<PagingLandScapeOctree> 
    {
    protected:
        virtual PagingLandScapeOctree* allocate ()
        {
            return new PagingLandScapeOctree();
        }
        virtual void deallocate (PagingLandScapeOctree *p)
        {
            delete p;
        }
    };


    //typedef std::list < WireBoundingBox* > BoxList;
    //typedef std::list < OcclusionBoundingBox* > BoxList;

    //typedef std::list < unsigned long > ColorList;
    //typedef std::list < SceneNode* > SceneNodeList;


    /** Specialized SceneManager that divides the geometry into an PagingLandScapeOctree in order to facilitate spatial queries.
    @remarks
    For debugging purposes, a special "CullCamera" can be defined.  To use it, call setUseCallCamera(true),
    and create a camera named "CullCamera".  All culling will be performed using that camera, instead of the viewport
    camera, allowing you to fly around and examine culling.
    */

    class PagingLandScapeOctreeSceneManager : public SceneManager
    {
        friend class PagingLandScapeOctreeIntersectionSceneQuery;
        friend class PagingLandScapeOctreeRaySceneQuery;
        friend class PagingLandScapeOctreeSphereSceneQuery;
        friend class PagingLandScapeOctreeAxisAlignedBoxSceneQuery;
        friend class PagingLandScapeOctreePlaneBoundedVolumeListSceneQuery;

    public:
        static int intersect_call;
        /** Standard Constructor.  Initializes the PagingLandScapeOctree to -500,-500,-500 to 500,500,500 with unlimited depth. */
        PagingLandScapeOctreeSceneManager(const String &name);
        /** Standard Constructor */
        PagingLandScapeOctreeSceneManager(const String &name, AxisAlignedBox& box, int max_depth);
        /** Standard destructor */
        virtual ~PagingLandScapeOctreeSceneManager(void);

       inline PagingLandScapeOctree * getNewOctree()
        {
            return mOctreeSet.getPoolable();
        }
        inline void deleteOctree(PagingLandScapeOctree * p)
        {
            p->reset ();
            mOctreeSet.removePoolable (p);
        }
        
        void shutdown();
        /// @copydoc SceneManager::getTypeName
        const String& getTypeName(void) const;

        /** Initializes the manager to the given box and depth.
        */
        void init(const AxisAlignedBox& box, int d);

        /** Creates a specialized PagingLandScapeOctreeNode */
        virtual	SceneNode* createSceneNode(void);
        /** Creates a specialized PagingLandScapeOctreeNode */
        virtual SceneNode* createSceneNode(const String& name);
        /** Creates a specialized PagingLandScapeOctreeCamera */
        virtual Camera* createCamera(const String& name);
        virtual void destroyCamera(Camera *cam);
        virtual void destroyCamera(const String& name);
        virtual void destroyAllCameras(void);

        void addCamera(Camera *cam);

        /** Deletes a scene node */
        virtual void destroySceneNode(const String& name);

        /** Does nothing more */
        virtual void _updateSceneGraph(Camera* cam);

        /** Recurses through the PagingLandScapeOctree determining which nodes are visible. */
#ifdef PLSM2_EIHORT
        virtual void _findVisibleObjects(Camera* cam, VisibleObjectsBoundsInfo *visibleBounds, bool onlyShadowCasters);
#else
        virtual void _findVisibleObjects(Camera* cam, bool onlyShadowCasters);
#endif

        /** Alerts each un-culled object, notifying it that it will be drawn.
        * Useful for doing calculations only on nodes that will be drawn, prior
        * to drawing them...
        */
        //virtual void _alertVisibleObjects(void);

        /** Walks through the PagingLandScapeOctree, adding any visible objects to the render queue.
        @remarks
	        If any octant in the PagingLandScapeOctree if completely within the the view frustum,
	        all sub-children are automatically added with no visibility tests.
        */
        void walkPagingLandScapeOctree(PagingLandScapeOctreeCamera * camera, RenderQueue * const queue,
                                        PagingLandScapeOctree * const octant, const bool foundvisible, const bool onlyShadowCasters);

        /** Checks the given PagingLandScapeOctreeNode, and determines if it needs to be moved
        * to a different octant.
        */
        void _updatePagingLandScapeOctreeNode(PagingLandScapeOctreeNode* nod);
        
	    /** Removes the given PagingLandScapeOctree node */
        void _removePagingLandScapeOctreeNode(PagingLandScapeOctreeNode* nod);
        
	    /** Adds the PagingLandScapeOctree Node, starting at the given PagingLandScapeOctree, and recursing at max to the specified depth.
        */
        void _addPagingLandScapeOctreeNode(PagingLandScapeOctreeNode* ocnod, PagingLandScapeOctree* PagingLandScapeOctree, int depth = 0);

	    /** Adds the PagingLandScapeOctree Node, starting at the given PagingLandScapeOctree, and recursing at max to the specified depth.
        */
        void _addPagingLandScapeOctreeMovableNode(PagingLandScapeOctreeNode* ocnod, PagingLandScapeOctree* PagingLandScapeOctree, int depth = 0);

	    /** Adds the PagingLandScapeOctree Node, starting at the given PagingLandScapeOctree, and recursing at max to the specified depth.
        */
        void _addPagingLandScapeOctreeStaticNode(PagingLandScapeOctreeNode* ocnod, PagingLandScapeOctree* PagingLandScapeOctree, int depth = 0);

        /** Recurses the PagingLandScapeOctree, adding any nodes intersecting with the box into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const AxisAlignedBox& box, std::list < SceneNode* > &list, 
            const SceneNode* const exclude = 0);

        /** Recurses the PagingLandScapeOctree, adding any nodes intersecting with the sphere into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const Sphere& sphere, std::list < SceneNode* > &list, 
            const SceneNode* const exclude = 0);

        /** Recurses the PagingLandScapeOctree, adding any nodes intersecting with the volume into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const PlaneBoundedVolume& volume, std::list < SceneNode* > &list, 
            const SceneNode* const exclude=0);

        /** Recurses the PagingLandScapeOctree, adding any nodes intersecting with the ray into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const Ray& ray, std::list < SceneNode* > &list, 
            const SceneNode* const exclude=0);

        /** Sets the box visibility flag */
        void setShowBoxes(bool b)
        {
            #ifdef _VISIBILITYDEBUG
                    mShowBoxes = b;
            #endif //_VISIBILITYDEBUG
        };

        /** Sets the cull camera flag */
        void setUseCullCamera(bool b)
        {
            #ifdef _VISIBILITYDEBUG
                    mCullCamera = b;
            #endif //_VISIBILITYDEBUG
        };

        /** Resizes the PagingLandScapeOctree to the given size */
        void resize(const AxisAlignedBox& box);
        void resize(const AxisAlignedBox &box, const int depth);

        /** Sets the given option for the SceneManager
                @remarks
            Options are:
            "Size", AxisAlignedBox *;
            "CullCamera", bool *;
            "Depth", int *;
            "ShowPagingLandScapeOctree", bool *;
        */

        virtual bool setOption(const String& key, const void* value);

        /** Gets the given option for the Scene Manager.
            @remarks
            See setOption
        */
        virtual bool getOption(const String& key, void* value);

        bool getOptionValues(const String& key, StringVector& refValueList);
        bool getOptionKeys(StringVector& refKeys);
        /** Overridden from SceneManager */
        void clearScene(void);

        AxisAlignedBoxSceneQuery* createAABBQuery(const AxisAlignedBox& box, unsigned long mask);
        SphereSceneQuery* createSphereQuery(const Sphere& sphere, unsigned long mask);
        PlaneBoundedVolumeListSceneQuery* createPlaneBoundedVolumeQuery(const PlaneBoundedVolumeList& volumes, unsigned long mask);
        RaySceneQuery* createRayQuery(const Ray& ray, unsigned long mask);
        IntersectionSceneQuery* createIntersectionQuery(unsigned long mask);

        void directRenderSingleQueue(RenderQueue *queue);
        void directRenderSingleObject(Renderable *mo);

        void addVisible(MovableObject *mo);

        const AxisAlignedBox &getBoundingBox()const {return mBox;};

        void registeredNodeInCamera(OcclusionElement *on);
		void unregisteredNodeInCamera(OcclusionElement *on);

        void registerCamera (PagingLandScapeOctreeCamera *c);
		void unregisterCamera(PagingLandScapeOctreeCamera *c);

    protected:

        /// The root PagingLandScapeOctree
        PagingLandScapeOctree* mPagingLandScapeOctree;

        typedef std::map<unsigned int, MovableObjectList * > VisiblesPerCam;
        VisiblesPerCam mVisibles;
        MovableObjectList * mCamInProgressVisibles;
        PolygonMode mCamDetail;
        bool mCamClear;

        /// number of rendered objects
        int mNumObjects;

        /// max depth for the tree.
        int mMaxDepth;

        /// Size of the PagingLandScapeOctree
        AxisAlignedBox mBox;


        // OCCLUSION 
        Occlusion mOcclusion;

        //DEBUG  INFO
        #ifdef _VISIBILITYDEBUG
            /// box visibility flag
            bool mShowBoxes;
            /// list of boxes to be rendered
            MovableObjectList mBoxes;
            /// cull camera flag
            bool mCullCamera;
            /// cull camera flag
            bool mCullDebug;

			String mDebugText;
        #endif //_VISIBILITYDEBUG


		PagingLandScapeOctreeCamera *mCurrentOptionCamera;

        RenderTexture     *mOcclusionDepth;
        Camera            *mOcclusionCamera;

        void enableHardwareOcclusionTests();
        void disableHardwareOcclusionTests();

        OctreeSet mOctreeSet;


    };


}

#endif

