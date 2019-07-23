-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
red = gr.material({1, 0, 0}, {1, 1, 1}, 50)
matte_red = gr.material({1, 0, 0}, {0,0,0}, 0)
shiny_green = gr.material({0.1,0.9,0.1}, {1,1,1}, 100)

scene = gr.node('scene')
-- scene:rotate('X', 23)
scene:translate(2, 1, -10)

-- -- the arc
-- arc = gr.node('arc')
-- scene:add_child(arc)
-- arc:translate(0,0,-10)
-- arc:rotate('Y', 60)
-- p1 = gr.cube('p1')
-- arc:add_child(p1)
-- p1:set_material(gold)
-- p1:scale(0.8, 4, 0.8)
-- p1:translate(-2.4, 0, -0.4)

p2 = gr.sphere('p2')
scene:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 2)
p2:translate(2, 0, -10)

p1 = gr.sphere('p1')
scene:add_child(p1)
p1:set_material(blue)
p1:scale(5, 5, 5)
p1:translate(-10,-6, -5)

p4 = gr.sphere('p4')
scene:add_child(p4)
p4:set_material(matte_red)
p4:scale(1, 1, 1)
p4:translate(0,0, -5)

p5 = gr.sphere('p5')
scene:add_child(p5)
p5:set_material(red)
p5:scale(1, 1, 1)
p5:translate(-1.5,-2, -1)


p6 = gr.sphere('p6')
scene:add_child(p6)
p6:set_material(gold)
p6:scale(0.2, 0.2, 0.2)
p6:translate(0,-2, 1.55)

p3 = gr.cube('p3')
scene:add_child(p3)
p3:set_material(grass)
p3:scale(5, 0.35, 5)
p3:rotate('Y', 30)
p3:rotate('X', 30)
p3:translate(-4,-3, -5)


-- s = gr.sphere('s')
-- arc:add_child(s)
-- s:set_material(gold)
-- s:scale(4, 0.6, 0.6)
-- s:translate(0, 4, 0)

-- -- the floor
plane = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(shiny_green)

plane:scale(15, 15, 15)
plane:rotate('Y', 90)
plane:rotate('X', 90)
plane:rotate('Y', 20)
plane:translate(-8, -4, -30)

-- sphere
poly = gr.mesh( 'poly', 'Assets/dodeca.obj' )
scene:add_child(poly)
poly:scale(0.2, 0.2, 0.2)
poly:translate(0, -2.5, 2)
poly:set_material(gold)

-- The lights
l1 = gr.light({5,0,-5}, {1, 1, 1}, {0, 0, 0.03})
l2 = gr.light({0, 40, -10}, {0.4, 0.4, 1}, {1, 0, 0})
l3 = gr.light({-5, 15, -10}, {1, 1, 1}, {10, 0, 0})

gr.render(scene, 'sample.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2, l3})
