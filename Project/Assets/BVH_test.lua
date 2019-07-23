-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.3, 0.3, 0.3}, 25, 0)
mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)

mat5 = gr.material({1.0, 0.6, 0.1}, {0,0,0}, 25, 0)


scene_root = gr.node('root')
scene_root:translate(-10,0,0);

n1 = gr.node('n1')
n1:translate(10,0,1)
scene_root:add_child(n1)

s1 = gr.nh_sphere('s1', {0, 0, 0}, 0.5)
n1:add_child(s1)
s1:set_material(mat1)

n2 = gr.node('n2')
n2:translate(3,0,0)
n1:add_child(n2)

s2 = gr.nh_sphere('s2', {5, 0, 0}, 0.5)
n2:add_child(s2)
s2:set_material(mat1)

n3 = gr.node('n3')
n3:translate(-1.5,5,3)
n2:add_child(n3)

for i=1,10 do
	-- n = gr.nh_sphere("e", {i*3, 0, 0},0.5)
	-- scene_root:add_child(gr.nh_sphere("e", {i*3, 0, 0},0.5))

	-- print(i)
	-- print('aaaaaaa')

	-- t = tonumber(i)
	-- print(t)
	-- print('------------')

	for j=1,6 do

		for k=1,3 do
			gg = gr.nh_sphere('gg', {i*3, j*2, k*2+ 5}, 0.5)

			-- gg = gr.nh_sphere('gg', {i*3, 3, 10}, 0.5)
			gg:set_material(mat1)
			scene_root:add_child(gg)
		end
	end
end

s3 = gr.nh_sphere('s3', {0, 0, 0}, 0.5)
n3:add_child(s3)
s3:set_material(mat1)

-- s2 = gr.nh_box('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat1)

plane = gr.mesh( 'plane', 'plane.obj' )
scene_root:add_child(plane)
ps = 800
plane:scale(ps, ps, ps)
-- plane:rotate('Y', 90)
-- plane:rotate('X', 90)
-- plane:rotate('Y', 20)
plane:translate(-8, -5, 0)
plane:set_material(mat1)

-- s6 = gr.cube('s6')
-- -- s6:
-- s6:rotate('y', 30)
-- s6:scale(25,25,25)
-- s6:translate(0, 0, 700)
-- scene_root:add_child(s6)
-- s6:set_material(mat4)

-- s6 = gr.mesh( 'poly', '../objs/grid_cube/grid_cube_bumpy.obj' )
-- s6:rotate('y', 45)
-- s6:rotate('x', -15)
-- s6:scale(25,25,25)
-- s6:translate(0, 30, 700)
-- scene_root:add_child(s6)
-- s6:set_material(mat5)

-- white_light = gr.light({200, 200, 0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- white_light2 = gr.light({0,0, 2000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- magenta_light = gr.light({0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

black_light = gr.light({20, 100, 0}, {1,1,1}, {2, 0, 0})

-- white_area_light = gr.area_light("spotlight ugh", {0.9,0.9,0.9}, {1,0,0})
-- ls = 100
-- white_area_light:rotate('x',-30)
-- white_area_light:rotate('y',90)

-- white_area_light:scale(ls,ls,ls)
-- white_area_light:translate(400,200, 100)

-- scene_root:add_child(white_area_light)



size = 512

gr.render(scene_root, 'simple.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {20, 5, 20}, {0, 0, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {black_light})


-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

-- gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
-- grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
-- blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)

-- scene = gr.node('scene')
-- scene:rotate('X', 23)
-- scene:translate(6, -2, -15)

-- -- the arc
-- arc = gr.node('arc')
-- scene:add_child(arc)
-- arc:translate(0,0,-10)
-- arc:rotate('Y', 60)

-- s = gr.sphere('s')
-- arc:add_child(s)
-- s:set_material(gold)
-- s:scale(4, 0.6, 0.6)
-- s:translate(0, 4, 0)

-- -- the floor
-- plane = gr.mesh( 'plane', 'plane.obj' )
-- scene:add_child(plane)
-- plane:set_material(grass)
-- plane:scale(30, 30, 30)

-- -- The lights
-- l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
-- l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

-- gr.render(scene, 'simple.png', 512, 512, 
-- 	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
-- 	  {0.4, 0.4, 0.4}, {l1, l2})

