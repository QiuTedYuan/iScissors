
#include "correlation.h"
#include "iScissor.h"
#include "fibheap.h"

#include <iostream>


// two inlined routined that may help;

inline Node &NODE(Node *n, int i, int j, int width)
{ return *(n+j*width+i); }

inline unsigned char PIXEL(const unsigned char *p, int i, int j, int c, int width)
{ return *(p+3*(j*width+i)+c); }

//p:image, i:col, j:row, c:RGB
inline double PIXEL(const double *p, int i, int j, int c, int width)
{
	return *(p + 3 * (j*width + i) + c);
}

/************************TO DO 1***************************
 *InitNodeBuf
 *	INPUT: 
 *		img:	a RGB image of size imgWidth by imgHeight;
 *		nodes:	a allocated buffer of Nodes of the same size, one node corresponds to a pixel in img;
 *  OUPUT:
 *      initializes the column, row, and linkCost fields of each node in the node buffer.  
 */

void InitNodeBuf(Node *nodes, const unsigned char *img, int imgWidth, int imgHeight)
{
	//printf("InitNodeBuf in iScissor.cpp: to be implemented!\n");
	for (int row = 0; row < imgHeight; ++row) {
		for (int col = 0; col < imgWidth; ++col) {
			Node& node = NODE(nodes, col, row, imgWidth);
			node.column = col;
			node.row = row;
			node.prevNode = NULL;
		}
	}
	double** d_link = new double*[8];
	double max_d = 0;
	for (int i = 0; i < 8; ++i) {
		d_link[i] = new double[imgWidth*imgHeight];
		double* result = new double[imgWidth*imgHeight * 3];
		//"For debugging perpose, you may want to scale down each link cost by a factor of 1.5 or 2 so that they can be converted to byte format without clamping to[0,255]"
		image_filter(result, img, NULL, imgWidth, imgHeight, kernels[i], 3, 3, 0.5, 0);
		for (int row = 0; row < imgHeight; ++row) {
			for (int col = 0; col < imgWidth; ++col) {
				double red = PIXEL(result, col, row, 0, imgWidth);
				double green = PIXEL(result, col, row, 1, imgWidth);
				double blue = PIXEL(result, col, row, 2, imgWidth);
				d_link[i][row*imgWidth + col] = sqrt((red*red + green*green + blue*blue) / 3);
				if (max_d < d_link[i][row*imgWidth + col]) {
					max_d = d_link[i][row*imgWidth + col];
				}
			}
		}
		delete[] result;
	}
	for (int row = 0; row < imgHeight; ++row) {
		for (int col = 0; col < imgWidth; ++col) {
			for (int i = 0; i < 8; ++i) {
				double d = d_link[i][row*imgWidth + col];
				double cost = (max_d - d)*((i % 2 == 0) ? 1 :SQRT2);
				NODE(nodes, col, row, imgWidth).linkCost[i] = cost;
			}
		}
	}
	for (int i = 1; i < 8; ++i) {
		delete[] d_link[i];
	}
	delete[] d_link;

}

/************************TO DO 3***************************
 *LiveWireDP:
 *	INPUT:
 *		seedX, seedY:	seed position in nodes
 *		nodes:			node buffer of size width by height;
 *      width, height:  dimensions of the node buffer;
 *		selection:		if selection != NULL, search path only in the subset of nodes[j*width+i] if selection[j*width+i] = 1;
 *						otherwise, search in the whole set of nodes. 
 *		numExpanded:		compute the only the first numExpanded number of nodes; (for debugging)
 *	OUTPUT:
 *		computes the minimum path tree from the seed node, by assigning 
 *		the prevNode field of each node to its predecessor along the minimum 
 *		cost path from the seed to that node.
 */

void LiveWireDP(int seedX, int seedY, Node *nodes, int width, int height, const unsigned char *selection, int numExpanded)
{
	//printf("LiveWireDp in iScissor.cpp: to be implemented!\n");
	// TBD: FibHeap pq;
	CTypedPtrHeap<Node> pq;
	int count = 0;
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			Node* current = &NODE(nodes, col, row, width);
			current->state = INITIAL;
		}
	}
	Node* seed = &NODE(nodes, seedX, seedY, width);
	seed->totalCost = 0;
	seed->prevNode = NULL;
	pq.Insert(seed);
	while (!pq.IsEmpty()) {
		Node* q = pq.ExtractMin();
		q->state = EXPANDED;
		++count;
		if (count == numExpanded) {
			return;
		}
		else {
			for (int i = 0; i < 8; ++i) {
				int x, y;
				q->nbrNodeOffset(x, y, i);
				int xCord = q->column + x;
				int yCord = q->row + y;

				if (xCord >= 0 && xCord < width && yCord >= 0 && yCord < height) {
					if (selection == NULL || selection[yCord*width + xCord] == 1) {
						Node* r = &NODE(nodes, xCord, yCord, width);
						if (r->state == INITIAL) {
							r->prevNode = q;
							r->totalCost = q->totalCost + q->linkCost[i];
							r->state = ACTIVE;
							pq.Insert(r);
						}
						else if (r->state==	ACTIVE) {
							if (r->totalCost > q->totalCost + q->linkCost[i]) {
								r->totalCost = q->totalCost + q->linkCost[i];
								r->prevNode = q;
								pq.Update(r);
							}
						}
					}
				}
			}
		}
	}


}

/************************TO DO 4***************************
 *MinimumPath:
 *	INPUT:
 *		nodes:				a node buffer of size width by height;
 *		width, height:		dimensions of the node buffer;
 *		freePtX, freePtY:	an input node position;
 *	OUTPUT:
 *		insert a list of nodes along the minimum cost path from the seed node to the input node.  
 *		Notice that the seed node in the buffer has a NULL predecessor.
 *		And you want to insert pointer to Node in path, e.g., 
 *		insert nodes+j*width+i if you want to insert node at (i,j), instead of nodes[nodes+j*width+i]!!!
 *		after the procedure, the seed should be the head of path and the input code should be the tail
 */

void MinimumPath(CTypedPtrDblList <Node> *path, int freePtX, int freePtY, Node *nodes, int width, int height)
{
	//printf("MinimumPath in iScissor.cpp: to be implemented!\n");
	Node* nodept = &NODE(nodes, freePtX, freePtY, width);
	if (!nodept) {
		return;
	}
	else {
		CTypedPtrDblElement <Node> *prev = path->AddHead(nodept);
		while (!nodept->prevNode) {
			nodept = nodept->prevNode;
			prev = path->AddNext(prev, nodept);
		}
	}
}

/************************An Extra Credit***************************
 *SeeSnap:
 *	INPUT:
 *		img:				a RGB image buffer of size width by height;
 *		width, height:		dimensions of the image buffer;
 *		x,y:				an input seed position;
 *	OUTPUT:
 *		update the value of x,y to the closest edge based on local image information.
 */

void SeedSnap(int &x, int &y, unsigned char *img, int width, int height)
{
	printf("SeedSnap in iScissor.cpp: to be implemented for extra credits!\n");
}

//generate a cost graph from original image and node buffer with all the link costs;
void MakeCostGraph(unsigned char *costGraph, const Node *nodes, const unsigned char *img, int imgWidth, int imgHeight)
{
	int graphWidth = imgWidth * 3;
	int graphHeight = imgHeight * 3;
	int dgX = 3;
	int dgY = 3*graphWidth;

	int i,j;
	for (j=0;j<imgHeight;j++)
	{
		for (i=0;i<imgWidth;i++)
		{
			int nodeIndex = j*imgWidth+i;
			int imgIndex = 3*nodeIndex;
			int costIndex = 3*((3*j+1)*graphWidth+(3*i+1));
			
			const Node *node = nodes+nodeIndex;
			const unsigned char *pxl = img + imgIndex;
			unsigned char *cst = costGraph + costIndex;

			cst[0] = pxl[0];
			cst[1] = pxl[1];
			cst[2] = pxl[2];

			
			//r,g,b channels are grad info in seperate channels;				
			DigitizeCost(cst	   + dgX, node->linkCost[0]);
			DigitizeCost(cst - dgY + dgX, node->linkCost[1]);
			DigitizeCost(cst - dgY      , node->linkCost[2]);
			DigitizeCost(cst - dgY - dgX, node->linkCost[3]);
			DigitizeCost(cst	   - dgX, node->linkCost[4]);
			DigitizeCost(cst + dgY - dgX, node->linkCost[5]);
			DigitizeCost(cst + dgY	   ,  node->linkCost[6]);
			DigitizeCost(cst + dgY + dgX, node->linkCost[7]);
		}
	}
}
