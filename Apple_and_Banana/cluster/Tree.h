#pragma once
#include<iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

inline Vec2d center(const Vec2d& el1, const Vec2d& el2)
{
    return Vec2d((el1.val[0] + el2.val[0]) / 2, (el1.val[1] + el2.val[1]) / 2);
}

template<typename Ty>
class TreeNode
{
public:
    using type = Ty;
    TreeNode<type> *left, *right;
    type* el;
    const bool deleteFlag;
    TreeNode(const bool& deFlag = 0):deleteFlag(deFlag)
    {
        left = NULL;
        right = NULL;
        el = NULL;
    }

    TreeNode(const type& e, TreeNode<type>*l = 0, TreeNode<type>*r = 0):deleteFlag(0)
    {
        el = new type;
        *el = e;
        left = l;
        right = r;
    }
    void set_el(const type& e)
    {
        el = new type;
        *el = e;
    }
    template<typename U>
    friend ostream& operator<<(ostream&, const TreeNode<U>&);
    ~TreeNode()
    {   
        /*if(el)
            delete el;*/
    }
};



template<typename Ty>
class Tree
{
public:
    using type = Ty;
    TreeNode<type> *root;
    template<typename U>
    friend ostream& operator<<(ostream&, const Tree<U>&);
    Tree()
    {
        root = NULL;
    }
    Tree(TreeNode<type>& r)
    {
        root = &r;
    }
    

    void joint( Tree<type> node1,  Tree<type> node2)
    {
        root = new TreeNode<type>(true);
        root->set_el(center(*(node1.root->el),*(node2.root->el)));
        push_after(*root, node1, node2);
    }

    void push_after(TreeNode<type>& r, Tree<type>& left,Tree<type>& right )//= (const TreeNode<type> *)NULL)
    {
        if (!r.left && !r.right)
        {
            r.left = left.root;
            r.right = right.root;
            
        }
        else
            throw("cannot push_after this tree node because it is full.");
    }
    void push_after(Tree<type>& r, Tree<type>& left)
    {
        if (!r.left )
        {
            r.left = left.root;
        }
        else if (!r.right)
        {
            r.right = left.root;
        }
        else
            throw("cannot push_after this tree node because it is full.");
    }
     
    void insert_left(TreeNode<type>& node,const type& el)
    {
        TreeNode<type>* ptr = node.left;
        node.left = new TreeNode<type>(true);
        push_after(node.left, ptr);
    }

    void insert_right(TreeNode<type>& node, const type& el)
    {
        TreeNode<type>* ptr = node.right;
        node.right = new TreeNode<type>(true);
        push_after(node.right, ptr);
    }

    void erase(TreeNode<type>* ptr)//É¾³ý×ÓÊ÷
    {
        TreeNode<type>*left = ptr->left, *right = ptr->right;
        if (ptr->deleteFlag)
        {
            //delete ptr;
        }
        else
        {
            ptr->left = NULL;
            ptr->right = NULL;
        }
        if (left)
            erase(left);
        if (right)
            erase(right);
    }
    ~Tree() 
    {
        erase(root);
    };//±éÀúÊý£¬deleteFlag
};
template<typename T>
ostream& operator<<(ostream& out, const TreeNode<T>& tr)
{
    out << *(tr.el);
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const Tree<T>& tr)
{
    out << *(tr.root) << std::endl;
    if (tr.root->left) 
        out << *(tr.root->left);
    if (tr.root->left&&tr.root->right)
        out << "/|\\";
    if (tr.root->right) 
        out << *(tr.root->right);
    if (tr.root->left || tr.root->right)
        out << std::endl;
    return out;
}