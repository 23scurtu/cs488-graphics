#include "BVH.hpp"
#include <algorithm>

void BVH::get_gnodes(SceneNode* root, vector<GeometryNode*> &result) const
{
    if(root->m_nodeType == NodeType::GeometryNode)
    {
        GeometryNode * geometryNode = static_cast<GeometryNode *>(root);
        result.push_back(geometryNode);
    }

    for(SceneNode * child : root->children) {
        get_gnodes(child, result);
    }
}

struct less_than_midpoint_x
{
    inline bool operator() (const pair<AABB, GeometryNode*>& b1, const pair<AABB, GeometryNode*>& b2)
    {
        return (b1.first.midpoint.x < b2.first.midpoint.x);
    }
};

struct less_than_midpoint_y
{
    inline bool operator() (const pair<AABB, GeometryNode*>& b1, const pair<AABB, GeometryNode*>& b2)
    {
        return (b1.first.midpoint.y < b2.first.midpoint.y);
    }
};

struct less_than_midpoint_z
{
    inline bool operator() (const pair<AABB, GeometryNode*>& b1, const pair<AABB, GeometryNode*>& b2)
    {
        return (b1.first.midpoint.z < b2.first.midpoint.z);
    }
};

BVHNode* BVH::construct_tree(vector<pair<AABB, GeometryNode*>> boxes)
{
    if(boxes.empty()) return nullptr;

    if(boxes.size() == 1)
    {
        auto result = new BVHNode();
        result->aabb = boxes[0].first;
        result->objects.push_back(boxes[0].second);
        if(boxes[0].second->m_primitive->type() == MESH)
            result->simple_primitive = false;


        return result;
    }

    if(boxes.size() <= 2)
    {
        auto result = new BVHNode();
        result->aabb = AABB(min_components(boxes[0].first.min, boxes[1].first.min),
                            max_components(boxes[0].first.max, boxes[1].first.max));
        result->objects.push_back(boxes[0].second);
        result->objects.push_back(boxes[1].second);

        if(boxes[0].second->m_primitive->type() == MESH || boxes[1].second->m_primitive->type() == MESH)
            result->simple_primitive = false;

        return result;
    }

    vec3 max = boxes[0].first.max;
    vec3 min = boxes[0].first.min;

    for(auto box: boxes)
    {
        max = max_components(max, box.first.max);
        min = min_components(min, box.first.min);
    }

    // cout << min.x << ", " << min.y << ", " << min.z<< ", " << endl;
    // cout << max.x << ", " << max.y << ", " << max.z<< ", " << endl;

    vec3 dim_lengths = max - min;

    if(dim_lengths.x >= dim_lengths.y && dim_lengths.x >= dim_lengths.z)
    {
        sort(boxes.begin(), boxes.end(), less_than_midpoint_x());
    }
    else if(dim_lengths.y >= dim_lengths.x && dim_lengths.y >= dim_lengths.z)
    {
        sort(boxes.begin(), boxes.end(), less_than_midpoint_y());
    }
    else if(dim_lengths.z >= dim_lengths.y && dim_lengths.z >= dim_lengths.x)
    {
        sort(boxes.begin(), boxes.end(), less_than_midpoint_z());
    }

    size_t half_size = boxes.size() / 2;
    vector<pair<AABB, GeometryNode*>> left(boxes.begin(), boxes.begin() + half_size);
    vector<pair<AABB, GeometryNode*>> right(boxes.begin() + half_size, boxes.end());


    auto result = new BVHNode(construct_tree(left), construct_tree(right));
    
    result->aabb = AABB(min, max);

    return result;
}

// Ensure root's world transforms have been calculated already!
BVH::BVH(SceneNode* root)
{
    vector<GeometryNode*> gnodes;
    get_gnodes(root, gnodes);

    vector<pair<AABB, GeometryNode*>> transformed_boxes;
    
    for(auto node: gnodes)
    {
        transformed_boxes.push_back(make_pair( node->m_primitive->aabb.transform(node->world_trans),node));
    }

    tree = construct_tree(transformed_boxes);
}

vector<GeometryNode*> BVH::candidate_geometry_nodes(vec3 eye, vec3 ray ) const
{
    vector<GeometryNode*> result;
    collide_subtree(eye, ray, tree, result);
    return result;
}

void BVH::collide_subtree(vec3 eye, vec3 ray, BVHNode* root, vector<GeometryNode*> &result) const
{
    if(root == nullptr) return;

    // If the node only contains primitives with simple intersection routines, 
    // do not check collision with their bounding boxes (jump to placing them 
    // in the collision candidate list)
    if(root->simple_primitive && root->l == nullptr && root->r == nullptr)
    {
        for(auto obj: root->objects) result.push_back(obj);
        return;
    }

    if(root->aabb.collide(eye, ray))
    {
        for(auto obj: root->objects) result.push_back(obj);

        collide_subtree(eye, ray, root->l, result);
        collide_subtree(eye, ray, root->r, result);
    }
}