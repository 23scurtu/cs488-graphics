-- Setup Cornell box

green_mat = gr.material({0.1,0.9,0.1}, r, 25, 0)
red_mat = gr.material({1,0.15,0.15}, r, 25, 0)
white_mat = gr.material({0.9, 0.9, 0.9}, r, 25, 0)

plane = gr.mesh( 'floor', '../plane.obj' )
root:add_child(plane)
ps = 200
plane:scale(ps, ps, ps)
plane:translate(0, 0, 0)
plane:set_material(white_mat)

plane = gr.mesh( 'back_wall', '../plane.obj' )
root:add_child(plane)
ps = 200
plane:scale(ps, ps, ps)
-- plane:rotate('Y', 90)
plane:rotate('X', 90)
-- plane:rotate('Y', 20)
plane:translate(0, 100, -100)
plane:set_material(white_mat)

-- plane = gr.mesh( 'behind_wall', '../plane.obj' )
-- root:add_child(plane)
-- ps = 200
-- plane:scale(ps, ps, ps)
-- -- plane:rotate('Y', 90)
-- plane:rotate('X', 90)
-- -- plane:rotate('Y', 20)
-- plane:translate(0, 100, 400)
-- plane:set_material(white_mat)

plane = gr.mesh( 'left_wall', '../plane.obj' )
root:add_child(plane)
ps = 200
plane:scale(ps, ps, ps)
plane:rotate('Z', -90)
plane:translate(-100, 100, 0)
plane:set_material(red_mat)

plane = gr.mesh( 'right_wall', '../plane.obj' )
root:add_child(plane)
ps = 200
plane:scale(ps, ps, ps)
plane:rotate('Z', 90)
plane:translate(100, 100, 0)
plane:set_material(green_mat)

plane = gr.mesh( 'roof', '../plane.obj' )
root:add_child(plane)
ps = 200
plane:rotate('Z', 180)
plane:scale(ps, ps, ps)
plane:translate(0, 200, 0)
plane:set_material(white_mat)