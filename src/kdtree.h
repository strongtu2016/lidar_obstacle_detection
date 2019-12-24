#include <iostream>


struct Node
{
	int id;
	Node* left;
	Node* right;

	Node(int setId)
	:	id(setId), left(NULL), right(NULL)
	{}
};


template <typename PointT>
struct KdTree
{
	Node* root;
	typename pcl::PointCloud<PointT>::Ptr cloud;  // reference to pcl::PointCloud<PointT> object

	KdTree(typename pcl::PointCloud<PointT>::Ptr pointcloud)
	: root(NULL), cloud(pointcloud)
	{}

    void insertHelper(Node** node, uint depth, int id)  // double pointer (Node**) because root was defined as Node* (node pointer) originally, and then we pass the memory address
	{
		/* Creates a new node and places it in the right position in the k-d tree */

		if (*node == NULL)  // dereferencing to get value  // terminates when it hits a null node
		{
			*node = new Node(id);  // pointing root pointer to new data  // TOTRY: use a pointer reference instead, then no need to dereference it (*node)
			std::cout << "Added id to tree" << std::endl;
			std::cout << "Depth: " << depth << std::endl;
		}
		else  // traverse
		{
			uint variable = depth % 3;  // 3D, always 0, 1 or 2

			if (variable == 0)  // point[0]: x value
            {
                if (cloud->points[id].x < cloud->points[(*node)->id].x)
                    insertHelper(&((*node)->left), depth + 1, id);  // passing address of dereferenced node's left child
                else
                    insertHelper(&((*node)->right), depth + 1, id);
            }
            else if (variable == 1)
            {
                if (cloud->points[id].y < cloud->points[(*node)->id].y)
                    insertHelper(&((*node)->left), depth + 1, id);
                else
                    insertHelper(&((*node)->right), depth + 1, id);
            }
            else if (variable == 2)
            {
                if (cloud->points[id].z < cloud->points[(*node)->id].z)
                    insertHelper(&((*node)->left), depth + 1, id);
                else
                    insertHelper(&((*node)->right), depth + 1, id);
            }
		}
    }
    
    void insertPointIndex(int id)
	{
		insertHelper(&root, 0, id);  // passing the address of root
	}

    void searchHelper(int id, Node** node, uint depth, float distanceTol, std::vector<int>& nearbyPoints)
	{
		std::cout << "\nid: " << id << std::endl;
		std::cout << "depth: " << depth << std::endl;

		if (*node != NULL)
		{
			// Checking if point in current node is inside the target box

			if (   (cloud->points[(*node)->id].x >= (cloud->points[id].x - distanceTol) && cloud->points[(*node)->id].x <= (cloud->points[id].x + distanceTol)) \
				&& (cloud->points[(*node)->id].y >= (cloud->points[id].y - distanceTol) && cloud->points[(*node)->id].y <= (cloud->points[id].y + distanceTol)) \
				&& (cloud->points[(*node)->id].z >= (cloud->points[id].z - distanceTol) && cloud->points[(*node)->id].z <= (cloud->points[id].z + distanceTol)) )
			{
				// Finding distance between 3D points (node x, y, z and target x, y, z)

				std::cout << "Point is in target box. Calculating distance..." << std::endl;

				float distance = sqrt( (cloud->points[(*node)->id].x - cloud->points[id].x) * (cloud->points[(*node)->id].x - cloud->points[id].x) + (cloud->points[(*node)->id].y - cloud->points[id].y) * (cloud->points[(*node)->id].y - cloud->points[id].y) + (cloud->points[(*node)->id].z >= (cloud->points[id].z - cloud->points[id].z)) * (cloud->points[(*node)->id].z >= (cloud->points[id].z - cloud->points[id].z)) );
				// std::cout << "distance: " << distance << std::endl;

				if (distance <= distanceTol)
				{
					std::cout << "Distance within distance tolerance threshold. Adding index to cluster..." << std::endl;
					nearbyPoints.push_back((*node)->id);  // prev: nearbyPoints.push_back(cloud->points[(*node)->id]);  // the point the node id refers to
				}
			}

			// TODO: check if indentation/ flow is correct

			// Checking box boundary to see where to move down next in the tree (left or right)

			uint varToCompare = depth % 3;
			std::cout << "Variable to compare: " << varToCompare << std::endl;

			if (varToCompare == 0)  // x
			{
				if ( (cloud->points[id].x - distanceTol) < cloud->points[(*node)->id].x )  // if <, that box is in the left region
					std::cout << "Moving down (left)" << std::endl;
					searchHelper(id, &((*node)->left), depth + 1, distanceTol, nearbyPoints);
				if ( (cloud->points[id].x + distanceTol) > cloud->points[(*node)->id].x )  // if left edge of the box is greater than the node's x or y value, then that box is in the right region
					std::cout << "Moving down (right)" << std::endl;
					searchHelper(id, &((*node)->right), depth + 1, distanceTol, nearbyPoints);
			}
			else if (varToCompare == 1)  // y
			{
				if ( (cloud->points[id].y - distanceTol) < cloud->points[(*node)->id].y )
					std::cout << "Moving down (left)" << std::endl;
					searchHelper(id, &((*node)->left), depth + 1, distanceTol, nearbyPoints);
				if ( (cloud->points[id].y + distanceTol) > cloud->points[(*node)->id].y )
					std::cout << "Moving down (right)" << std::endl;
					searchHelper(id, &((*node)->right), depth + 1, distanceTol, nearbyPoints);
			}
			else if (varToCompare == 2)  // z
			{
				if ( (cloud->points[id].z - distanceTol) < cloud->points[(*node)->id].z )
					std::cout << "Moving down (left)" << std::endl;
					searchHelper(id, &((*node)->left), depth + 1, distanceTol, nearbyPoints);
				if ( (cloud->points[id].z + distanceTol) > cloud->points[(*node)->id].z )
					std::cout << "Moving down (right)" << std::endl;
					searchHelper(id, &((*node)->right), depth + 1, distanceTol, nearbyPoints);
			}
		}
    }

    std::vector<int> search(int id, float distanceTol)
	{
        std::vector<int> nearbyPoints;  // TODO: make it a pointer

		searchHelper(id, &root, 0, distanceTol, nearbyPoints);

		return nearbyPoints;
    }
};