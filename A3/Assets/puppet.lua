-- -- Simple Scene:
-- -- An extremely simple scene that will render right out of the box
-- -- with the provided skeleton code.  It doesn't rely on hierarchical
-- -- transformations.

-- -- Create the top level root node named 'root'.
-- rootNode = gr.node('root')

-- -- Create a GeometryNode with MeshId = 'cube', and name = 'torso'.
-- -- MeshId's must reference a .obj file that has been previously loaded into
-- -- the MeshConsolidator instance within A3::init().
-- cubeMesh = gr.mesh('cube', 'name-of-cube')
-- cubeMesh:scale(1.0, 1.0, 1.0)
-- cubeMesh:rotate('y', 45.0)
-- cubeMesh:translate(0.0, -0.5, -5.0)
-- cubeMesh:set_material(gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0))

-- -- Add the cubeMesh GeometryNode to the child list of rootnode.
-- rootNode:add_child(cubeMesh)

-- -- Create a GeometryNode with MeshId = 'sphere', and name = 'head'.
-- sphereMesh = gr.mesh('sphere', 'name-of-sphere')
-- sphereMesh:scale(0.5, 0.5, 0.5)
-- sphereMesh:translate(0.0, 0.5, -5.0)
-- sphereMesh:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))

-- -- Add the sphereMesh GeometryNode to the child list of rootnode.
-- rootNode:add_child(sphereMesh)

-- -- sphereJoint = gr.joint('main_joint', {-60, 0, 1}, {-60, 0, 1});
-- -- sphereJoint:translate(0.0,0.5)
-- -- rootNode:add_child(sphereJoint)

-- mainJoint = gr.joint('main_joint', {-90, 0, 0}, {-90, 0, 0})
-- mainJoint:translate(1.0, 0.5, -5.0)
-- rootNode:add_child(mainJoint)

-- -- Create a GeometryNode with MeshId = 'sphere', and name = 'head'.
-- tailMesh = gr.mesh('cube', 'name-of-sphere')
-- tailMesh:scale(0.5, 0.5, 0.5)
-- -- tailMesh:translate(1.0, 0.5, -5.0)
-- tailMesh:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))

-- -- Add the sphereMesh GeometryNode to the child list of rootnode.
-- mainJoint:add_child(tailMesh)




-- -- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- -- equal to the return value from this Lua script.
-- return rootNode

-- Simple Scene:
-- An extremely simple scene that will render right out of the box
-- with the provided skeleton code.  It doesn't rely on hierarchical
-- transformations.

-- Create the top level root node named 'root'.
rootNode = gr.node('root')
rootNode:translate(0.0,0.0,-15);

-- Create a GeometryNode with MeshId = 'cube', and name = 'torso'.
-- MeshId's must reference a .obj file that has been previously loaded into
-- the MeshConsolidator instance within A3::init().
cubeMesh = gr.mesh('cube', 'body')
cubeMesh:scale(2,1, 1.0)
cubeMesh:rotate('y', 0)
-- cubeMesh:translate(0.0, -0.5, -5.0)
cubeMesh:set_material(gr.material({113/255,112/255,82/255}, {0.5, 0.5, 0.5}, 10.0))

rootNode:add_child(cubeMesh)

shoulderJoint = gr.joint('shoulder_joint', {-30, 0, 30}, {0,0,0})
shoulderJoint:translate(0, 1, 0)
shoulderJoint:rotate('z', 90)
rootNode:add_child(shoulderJoint)

hipsJoint = gr.joint('hips_joint', {-30, 0, 30}, {0,0,0})
hipsJoint:translate(0, -1, 0)
hipsJoint:rotate('z', 90)
rootNode:add_child(hipsJoint)

neckJoint = gr.joint('neck_joint', {-45, 0, 45}, {0, 0, 0})
neckJoint:translate(1, 0, 0)
neckJoint:rotate('x', 90)
-- neckJoint:rotate('y', 180)
-- neckJoint:rotate('z', 90)
shoulderJoint:add_child(neckJoint)

headJoint = gr.joint('head_joint', {-16, 0, 45}, {-45, 0, 45})
headJoint:translate(-0.25, 0, 0)
headJoint:rotate('z',90)
headJoint:rotate('y',90)
neckJoint:add_child(headJoint)

-- ARMS
leftUpperArmJoint = gr.joint('left_upper_arm_joint', {-30, 0, 180}, {0,0,0})
leftUpperArmJoint:translate(0, 1.75, 0)
leftUpperArmJoint:rotate('z', 90)
shoulderJoint:add_child(leftUpperArmJoint)

rightUpperArmJoint = gr.joint('right_upper_arm_joint', {-30, 0, 180}, {0,0,0})
rightUpperArmJoint:translate(0, -1.75, 0)
rightUpperArmJoint:rotate('z', 90)
shoulderJoint:add_child(rightUpperArmJoint)


leftLowerArmJoint = gr.joint('left_lower_arm_joint', {0, 0, 135}, {0,0,0})
leftLowerArmJoint:translate(0, 1.25, 0)
leftUpperArmJoint:add_child(leftLowerArmJoint)

rightLowerArmJoint = gr.joint('right_lower_joint', {0, 0, 135}, {0,0,0})
rightLowerArmJoint:translate(0, 1.25, 0)
rightUpperArmJoint:add_child(rightLowerArmJoint)

leftHandJoint = gr.joint('left_hand_joint', {-90, 0, 90}, {0,0,0})
leftHandJoint:translate(0, 1.25, 0)
leftLowerArmJoint:add_child(leftHandJoint)

rightHandJoint = gr.joint('right_hand_joint', {-90, 0, 90}, {0,0,0})
rightHandJoint:translate(0, 1.25, 0)
rightLowerArmJoint:add_child(rightHandJoint)

-- LEGS
leftUpperLegJoint = gr.joint('left_upper_leg_joint', {-45, 0, 90}, {0,0,0})
leftUpperLegJoint:translate(0, 0.75, 0)
leftUpperLegJoint:rotate('z', 90)
hipsJoint:add_child(leftUpperLegJoint)

leftLowerLegJoint = gr.joint('left_lower_leg_joint', {-135, 0, 0}, {0,0,0})
leftLowerLegJoint:translate(0, 2.75, 0)
leftUpperLegJoint:add_child(leftLowerLegJoint)

leftFootJoint = gr.joint('left_foot_joint', {-90, 0, 45}, {0,0,0})
leftFootJoint:translate(0, 1.6, 0.25)
leftLowerLegJoint:add_child(leftFootJoint)

rightUpperLegJoint = gr.joint('right_upper_leg_joint', {-45, 0, 90}, {0,0,0})
rightUpperLegJoint:translate(0, -0.75, 0)
rightUpperLegJoint:rotate('z', 90)
hipsJoint:add_child(rightUpperLegJoint)

rightLowerLegJoint = gr.joint('right_lower_leg_joint', {-135, 0, 0}, {0,0,0})
rightLowerLegJoint:translate(0, 2.75, 0)
rightUpperLegJoint:add_child(rightLowerLegJoint)

rightFootJoint = gr.joint('right_foot_joint', {-90, 0, 45}, {0,0,0})
rightFootJoint:translate(0, 1.6, 0.25)
rightLowerLegJoint:add_child(rightFootJoint)

---------------------------------------

shoulderMesh = gr.mesh('cube', 'shoulders')
shoulderMesh:scale(1,3, 1.0)
shoulderMesh:rotate('y', 0)
-- cubeMesh:translate(0.0, -0.5, -5.0)
shoulderMesh:set_material(gr.material({81/255,59/255,49/255}, {0.0, 0.0, 0.0}, 10.0))
shoulderJoint:add_child(shoulderMesh)

hipsMesh = gr.mesh('cube', 'hips')
hipsMesh:scale(1,2.5, 1.0)
hipsMesh:rotate('y', 0)
-- cubeMesh:translate(0.0, -0.5, -5.0)
hipsMesh:set_material(gr.material({1,1,1}, {0,0,0}, 10.0))
hipsJoint:add_child(hipsMesh)


neckMesh = gr.mesh('sphere', 'neck')
neckMesh:scale(1, 0.4, 0.4)
neckMesh:translate(-0.5, 0, 0)
neckMesh:set_material(gr.material({249/255,213/255,170/255}, {0.1, 0.1, 0.1}, 10.0))
neckJoint:add_child(neckMesh)

headMesh = gr.mesh('cube', 'head')
headMesh:scale(1.2, 1.2, 1.2)
headMesh:translate(0, -0.75, 0)
headMesh:set_material(gr.material({249/255,213/255,170/255}, {0.1, 0.1, 0.1}, 10.0))

-- Add the cubeMesh GeometryNode to the child list of rootnode.
headJoint:add_child(headMesh)

leftEye = gr.mesh('sphere', 'left_eye')
leftEye:scale(0.1, 0.1, 0.1)
leftEye:translate(0.3, -0.75, 0.6)
leftEye:set_material(gr.material({1, 1, 1}, {1, 1, 1}, 10.0))
headJoint:add_child(leftEye)

rightEye = gr.mesh('sphere', 'right_eye')
rightEye:scale(0.1, 0.1, 0.1)
rightEye:translate(-0.3, -0.75, 0.6)
rightEye:set_material(gr.material({1, 1, 1}, {1, 1, 1}, 10.0))
headJoint:add_child(rightEye)


leftUpperArmMesh = gr.mesh('cube', 'left_upper_arm')
leftUpperArmMesh:scale(0.5,1.5, 0.5)
leftUpperArmMesh:translate(0,0.25, 0)
leftUpperArmMesh:rotate('y', 0)
-- cubeMesh:translate(0.0, -0.5, -5.0)
leftUpperArmMesh:set_material(gr.material({81/255,59/255,49/255}, {0.0, 0.0, 0.0}, 10.0))
leftUpperArmJoint:add_child(leftUpperArmMesh)

rightUpperArmMesh = gr.mesh('cube', 'right_upper_arm')
rightUpperArmMesh:scale(0.5,1.5, 0.5)
rightUpperArmMesh:translate(0,0.25, 0)
rightUpperArmMesh:rotate('y', 0)
rightUpperArmMesh:set_material(gr.material({81/255,59/255,49/255}, {0.0, 0.0, 0.0}, 10.0))
rightUpperArmJoint:add_child(rightUpperArmMesh)

leftLowerArmMesh = gr.mesh('cube', 'left_lower_arm')
leftLowerArmMesh:scale(0.5,1.5, 0.5)
leftLowerArmMesh:translate(0,0.5, 0)
leftLowerArmMesh:rotate('y', 0)
leftLowerArmMesh:set_material(gr.material({81/255,59/255,49/255}, {0.0, 0.0, 0.0}, 10.0))
leftLowerArmJoint:add_child(leftLowerArmMesh)

rightLowerArmMesh = gr.mesh('cube', 'left_lower_arm')
rightLowerArmMesh:scale(0.5,1.5, 0.5)
rightLowerArmMesh:translate(0,0.5, 0)
rightLowerArmMesh:rotate('y', 0)
rightLowerArmMesh:set_material(gr.material({81/255,59/255,49/255}, {0.0, 0.0, 0.0}, 10.0))
rightLowerArmJoint:add_child(rightLowerArmMesh)

leftHandMesh = gr.mesh('cube', 'left_hand')
leftHandMesh:scale(0.7,1, 0.2)
leftHandMesh:translate(0,0.5, 0)
leftHandMesh:rotate('y', 0)
leftHandMesh:set_material(gr.material({249/255,213/255,170/255}, {0.1, 0.1, 0.1}, 10.0))
leftHandJoint:add_child(leftHandMesh)

rightHandMesh = gr.mesh('cube', 'left_hand')
rightHandMesh:scale(0.7,1, 0.2)
rightHandMesh:translate(0,0.5, 0)
rightHandMesh:rotate('y', 0)
rightHandMesh:set_material(gr.material({249/255,213/255,170/255}, {0.1, 0.1, 0.1}, 10.0))
rightHandJoint:add_child(rightHandMesh)


-- LEGS

leftUpperLegMesh = gr.mesh('cube', 'left_upper_leg')
leftUpperLegMesh:scale(0.75,2.5, 0.75)
leftUpperLegMesh:translate(0,1.25, 0)
leftUpperLegMesh:rotate('y', 0)
leftUpperLegMesh:set_material(gr.material({1,1,1}, {0,0,0}, 10.0))
leftUpperLegJoint:add_child(leftUpperLegMesh)

leftLowerLegMesh = gr.mesh('cube', 'left_lower_leg')
leftLowerLegMesh:scale(0.75,2, 0.75)
leftLowerLegMesh:translate(0,0.75, 0)
leftLowerLegMesh:rotate('y', 0)
leftLowerLegMesh:set_material(gr.material({1,1,1}, {0,0,0}, 10.0))
leftLowerLegJoint:add_child(leftLowerLegMesh)

leftFootMesh = gr.mesh('cube', 'left_foot')
leftFootMesh:scale(0.75,1, 0.4)
leftFootMesh:translate(0,0.5, 0)
leftFootMesh:rotate('x', 90)
leftFootMesh:set_material(gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 100.0))
leftFootJoint:add_child(leftFootMesh)

rightUpperLegMesh = gr.mesh('cube', 'right_upper_leg')
rightUpperLegMesh:scale(0.75,2.5, 0.75)
rightUpperLegMesh:translate(0,1.25, 0)
rightUpperLegMesh:rotate('y', 0)
rightUpperLegMesh:set_material(gr.material({1,1,1}, {0,0,0}, 10.0))
rightUpperLegJoint:add_child(rightUpperLegMesh)

rightLowerLegMesh = gr.mesh('cube', 'right_lower_leg')
rightLowerLegMesh:scale(0.75,2, 0.75)
rightLowerLegMesh:translate(0,0.75, 0)
rightLowerLegMesh:rotate('y', 0)
rightLowerLegMesh:set_material(gr.material({1,1,1}, {0,0,0}, 10.0))
rightLowerLegJoint:add_child(rightLowerLegMesh)

rightFootMesh = gr.mesh('cube', 'right_foot')
rightFootMesh:scale(0.75,1, 0.4)
rightFootMesh:translate(0,0.5, 0)
rightFootMesh:rotate('x', 90)
rightFootMesh:set_material(gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0))
rightFootJoint:add_child(rightFootMesh)


--------------------------------------------------------------------------

-- -- Create a GeometryNode with MeshId = 'sphere', and name = 'head'.
-- sphereMesh = gr.mesh('sphere', 'name-of-sphere')
-- sphereMesh:scale(0.5, 0.5, 0.5)
-- sphereMesh:translate(0.0, 0.5, -5.0)
-- sphereMesh:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))

-- -- Add the sphereMesh GeometryNode to the child list of rootnode.
-- rootNode:add_child(sphereMesh)

-- -- sphereJoint = gr.joint('main_joint', {-60, 0, 1}, {-60, 0, 1});
-- -- sphereJoint:translate(0.0,0.5)
-- -- rootNode:add_child(sphereJoint)

-- mainJoint = gr.joint('main_joint', {-90, 0, 0}, {-90, 0, 0})
-- mainJoint:translate(1.0, 0.5, -5.0)
-- rootNode:add_child(mainJoint)

-- -- Create a GeometryNode with MeshId = 'sphere', and name = 'head'.
-- tailMesh = gr.mesh('cube', 'name-of-sphere')
-- tailMesh:scale(0.5, 0.5, 0.5)
-- -- tailMesh:translate(1.0, 0.5, -5.0)
-- tailMesh:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))

-- -- Add the sphereMesh GeometryNode to the child list of rootnode.
-- mainJoint:add_child(tailMesh)




-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode

