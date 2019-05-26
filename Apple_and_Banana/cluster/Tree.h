#pragma once


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
    ~TreeNode()
    {   
        if(el)
            delete el;
    }
};

template<typename Ty>
class Tree
{
public:
    using type = Ty;
    TreeNode<type> *root;
    Tree()
    {
        root = NULL;
    }
    Tree(TreeNode<type>& r)
    {
        root = &r;
    }
    void joint(type el1, type el2)
    {
        TreeNode<type>* node1 = new TreeNode<type>(true);
        TreeNode<type>* node2 = new TreeNode<type>(true);
        node1->set_el(el1);
        node2->set_el(el2);
        root = new TreeNode<type>(true);
        root->set_el((el1 + el2) / 2);
        push_after(&root, node1, node2);
    }
    void push_after(TreeNode<type>& r, const TreeNode<type>& left,const TreeNode<type>& right )//= (const TreeNode<type> *)NULL)
    {
        if (!r.left && !r.right)
        {
            r.left = *left;
            r.right = *right;
            
        }
        else
            throw("cannot push_after this tree node because it is full.");
    }
    void push_after(TreeNode<type>& r, const TreeNode<type>& left)
    {
        if (!r.left )
        {
            r.left = *left;
        }
        else if (!r.right)
        {
            r.right = *left;
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
            delete ptr;
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

