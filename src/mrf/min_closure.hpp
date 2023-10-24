/*
Copyright (c) Lukas Esteban Gutierrez Lisboa

Parts of the code were copied or adapted from Pseudoflow Parametric Maximum Flow Solver.

The source code is subject to the following academic license. Note this is not an open source license.

Copyright © 2001. The Regents of the University of California (Regents). All Rights Reserved.

Permission to use, copy, modify, and distribute this software and its documentation for educational,
research, and not-for-profit purposes, without fee and without a signed licensing agreement, is hereby granted, provided that the above copyright notice,
this paragraph and the following two paragraphs appear in all copies, modifications, and distributions.
Contact The Office of Technology Licensing, UC Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620, (510) 643-7201,
for commercial licensing opportunities.

Created by Bala Chandran and Dorit S. Hochbaum, Department of Industrial Engineering and Operations Research, University of California, Berkeley.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/


#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <map>

typedef long long int llint;


struct node;

typedef struct arc
{
	struct node *from;
	struct node *to;
	float flow;
	float capacity;
	int direction;
	float *capacities;
} Arc;

typedef struct node
{
	int visited;
	int numAdjacent;
	int number;
	int label;
	float excess;
	struct node *parent;
	struct node *childList;
	struct node *nextScan;
	int numOutOfTree;
	Arc **outOfTree;
	int nextArc;
	Arc *arcToParent;
	struct node *next;
	struct node *prev;
	int breakpoint;
} Node;


typedef struct root
{
	Node *start;
	Node *end;
} Root;

//---------------  Global variables ------------------
static int numNodes = 0;
static int numArcs = 0;
static int source = 0;
static int sink = 0;
static int numParams = 100;

static int highestStrongLabel = 1;

static Node *adjacencyList = NULL;
static Root *strongRoots = NULL;
static int *labelCount = NULL;
static Arc *arcList = NULL;
//-----------------------------------------------------

#ifdef STATS
static llint numPushes = 0;
static int numMergers = 0;
static int numRelabels = 0;
static int numGaps = 0;
static llint numArcScans = 0;
#endif

static void
initializeNode (Node *nd, const int n)
{
	nd->label = 0;
	nd->excess = 0;
	nd->parent = NULL;
	nd->childList = NULL;
	nd->nextScan = NULL;
	nd->nextArc = 0;
	nd->numOutOfTree = 0;
	nd->arcToParent = NULL;
	nd->next = NULL;
	nd->prev = NULL;
	nd->visited = 0;
	nd->numAdjacent = 0;
	nd->number = n;
	nd->outOfTree = NULL;
	nd->breakpoint = (numParams+1);
}

static void
initializeRoot (Root *rt)
{
	rt->start = (Node *) malloc (sizeof(Node));
	rt->end = (Node *) malloc (sizeof(Node));

	if ((rt->start == NULL) || (rt->end == NULL))
	{
		printf ("%s Line %d: Out of memory\n", __FILE__, __LINE__);
		exit (1);
	}

	initializeNode (rt->start, 0);
	initializeNode (rt->end, 0);

	rt->start->next = rt->end;
	rt->end->prev = rt->start;
}


static void
freeRoot (Root *rt)
{
	free(rt->start);
	rt->start = NULL;

	free(rt->end);
	rt->end = NULL;
}

static void
liftAll (Node *rootNode, const int theparam)
{
	Node *temp, *current=rootNode;

	current->nextScan = current->childList;

	-- labelCount[current->label];
	current->label = numNodes;
	current->breakpoint = (theparam+1);

	for ( ; (current); current = current->parent)
	{
		while (current->nextScan)
		{
			temp = current->nextScan;
			current->nextScan = current->nextScan->next;
			current = temp;
			current->nextScan = current->childList;

			-- labelCount[current->label];
			current->label = numNodes;
			current->breakpoint = (theparam+1);
		}
	}
}

static void
addToStrongBucket (Node *newRoot, Node *rootEnd)
{
	newRoot->next = rootEnd;
	newRoot->prev = rootEnd->prev;
	rootEnd->prev = newRoot;
	newRoot->prev->next = newRoot;
}

static void
createOutOfTree (Node *nd)
{
	if (nd->numAdjacent)
	{
		if ((nd->outOfTree = (Arc **) malloc (nd->numAdjacent * sizeof (Arc *))) == NULL)
		{
			printf ("%s Line %d: Out of memory\n", __FILE__, __LINE__);
			exit (1);
		}
	}
}

static void
initializeArc (Arc *ac)
{
	int i;
	ac->from = NULL;
	ac->to = NULL;
	ac->capacity = 0;
	ac->flow = 0;
	ac->direction = 1;
	ac->capacities = NULL;
}

static void
addOutOfTreeNode (Node *n, Arc *out)
{
	n->outOfTree[n->numOutOfTree] = out;
	++ n->numOutOfTree;
}

static void graphInput(std::unordered_map<int,  std::unordered_map<int, long double> > adjacency_list, int graph_size, int n_var)  {
	int i = 0;
	Arc *ac = NULL;
	numNodes = graph_size + 2;
	numArcs = 0;
	
	for(auto& [key, value] : adjacency_list) {
		numArcs += value.size();
	}
	numArcs += graph_size * 2;
	if ((adjacencyList = (Node *) malloc (numNodes * sizeof (Node))) == NULL) {
		printf ("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
		exit (1);
	}

	if ((strongRoots = (Root *) malloc (numNodes * sizeof (Root))) == NULL) {
		printf ("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
		exit (1);
	}

	if ((labelCount = (int *) malloc (numNodes * sizeof (int))) == NULL) {
		printf ("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
		exit (1);
	}

	if ((arcList = (Arc *) malloc (numArcs * sizeof (Arc))) == NULL) {
		printf ("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
		exit (1);
	}
	
	for (i=0; i < numNodes; ++i) {
		initializeRoot (&strongRoots[i]);
		initializeNode (&adjacencyList[i], (i+1));
		labelCount[i] = 0;
	}
	
	i = 0;
	for(auto& [key, adj_nodes] : adjacency_list) {
		int l_key = key + 1;
		for(auto & [adj_node, adj_value]: adj_nodes) {
			initializeArc (&arcList[i]);
			ac = &arcList[i];
			ac->from = &adjacencyList[key - 1];
			ac->to = &adjacencyList[adj_node - 1];
			if ((ac->capacities = (float *) malloc (sizeof (float))) == NULL) {
				printf ("%s Line %d: Out of memory\n", __FILE__, __LINE__);
				exit (1);
			}
			ac->capacities[0] = adj_value * n_var;
			i++;
			++ ac->from->numAdjacent;
			++ ac->to->numAdjacent;
		}
	}
	
	source = numNodes - 1;
	sink = numNodes;
	int k = i;
	float initialValue = 1.0/(graph_size - n_var);
	float lambdaVals[numParams];
	for (int lambda = 0; lambda < numParams; lambda++) {
		lambdaVals[lambda] = (1.0/std::max(n_var, graph_size - n_var)) * (lambda/ (numParams/5));
	}
	
	// source
	for (i=1; i <= graph_size; ++i)  {
		initializeArc(&arcList[k]);
		ac = &arcList[k];
		initialValue = 0;
		for (auto& [key, value]: adjacency_list[i]) {
			initialValue += (float)value;
		}
		
		if (i <= (n_var * 2)) initialValue *= 1.0/n_var ;
		else initialValue *= 1.0/(graph_size - n_var);
		ac->from = &adjacencyList[source - 1];
		if ((ac->capacities = (float *) malloc (numParams * sizeof (float))) == NULL) {
			printf ("%s Line %d: Out of memory\n", __FILE__, __LINE__);
			exit (1);
		}
		for (int cap = numParams - 1; cap >= 0; cap--) {
			ac->capacities[cap] = std::max(initialValue -lambdaVals[numParams - 1 - cap], (float)0.0);
		}
		ac->to = &adjacencyList[i - 1];
		k++;
		++ ac->from->numAdjacent;
		++ ac->to->numAdjacent;
	}
	
	// sink
	for (i=1 ; i <= graph_size; ++i) {
		initializeArc (&arcList[k]);
		for (auto& [key, value]: adjacency_list[i]) {
			initialValue += (float)value;
		}
		if (i <= (n_var * 2)) initialValue *= 1.0/n_var ;
		else initialValue *= 1.0/(graph_size - n_var);
		ac = &arcList[k];
		ac->from = &adjacencyList[i - 1];
		if ((ac->capacities = (float *) malloc (numParams * sizeof (float))) == NULL) {
			printf ("%s Line %d: Out of memory\n", __FILE__, __LINE__);
			exit (1);
		}
		for (int cap = numParams - 1; cap >= 0; cap--) {
			ac->capacities[cap] = std::min(lambdaVals[numParams - 1 - cap] - initialValue, (float)0.0);
		}
		ac->to = &adjacencyList[sink - 1];	
		k++;
		++ ac->from->numAdjacent;
		++ ac->to->numAdjacent;
	}
	
	int capacity, numLines = 0, from, to, first=0, j;

	for (i=0; i<numNodes; ++i)  {
		createOutOfTree (&adjacencyList[i]);
	}
	
	for (i=0; i<numArcs; i++)  {
		to = arcList[i].to->number;
		from = arcList[i].from->number;
		capacity = arcList[i].capacity;
		if (!((source == to) || (sink == from) || (from == to))) {
			if ((source == from) && (to == sink)) {
				arcList[i].flow = capacity;
			}
			else if (from == source) {
				addOutOfTreeNode (&adjacencyList[from-1], &arcList[i]);
			}
			else if (to == sink) {
				addOutOfTreeNode (&adjacencyList[to-1], &arcList[i]);
			}
			else {
				addOutOfTreeNode (&adjacencyList[from-1], &arcList[i]);
			}
		}
	}
}

static void
simpleInitialization (void)
{
	int i, size;
	Arc *tempArc;

	size = adjacencyList[source-1].numOutOfTree;
	for (i=0; i<size; ++i)
	{
		tempArc = adjacencyList[source-1].outOfTree[i];
		tempArc->flow = tempArc->capacity;
		tempArc->to->excess += tempArc->capacity;
	}

	size = adjacencyList[sink-1].numOutOfTree;
	for (i=0; i<size; ++i)
	{
		tempArc = adjacencyList[sink-1].outOfTree[i];
		tempArc->flow = tempArc->capacity;
		tempArc->from->excess -= tempArc->capacity;
	}

	adjacencyList[source-1].excess = 0;
	adjacencyList[sink-1].excess = 0;

	for (i=0; i<numNodes; ++i) {
		if (adjacencyList[i].excess > 0)
		{
		    adjacencyList[i].label = 1;
			++ labelCount[1];

			addToStrongBucket (&adjacencyList[i], strongRoots[1].end);
		}
	}

	adjacencyList[source-1].label = numNodes;
	adjacencyList[source-1].breakpoint = 0;
	adjacencyList[sink-1].label = 0;
	adjacencyList[sink-1].breakpoint = (numParams+2);
	labelCount[0] = (numNodes - 2) - labelCount[1];
}

static inline int
addRelationship (Node *newParent, Node *child)
{
	child->parent = newParent;
	child->next = newParent->childList;
	newParent->childList = child;

	return 0;
}

static inline void
breakRelationship (Node *oldParent, Node *child)
{
	Node *current;

	child->parent = NULL;

	if (oldParent->childList == child)
	{
		oldParent->childList = child->next;
		child->next = NULL;
		return;
	}

	for (current = oldParent->childList; (current->next != child); current = current->next);

	current->next = child->next;
	child->next = NULL;
}

static void
merge (Node *parent, Node *child, Arc *newArc)
{
	Arc *oldArc;
	Node *current = child, *oldParent, *newParent = parent;

#ifdef STATS
	++ numMergers;
#endif

	while (current->parent)
	{
		oldArc = current->arcToParent;
		current->arcToParent = newArc;
		oldParent = current->parent;
		breakRelationship (oldParent, current);
		addRelationship (newParent, current);
		newParent = current;
		current = oldParent;
		newArc = oldArc;
		newArc->direction = 1 - newArc->direction;
	}

	current->arcToParent = newArc;
	addRelationship (newParent, current);
}


static inline void
pushUpward (Arc *currentArc, Node *child, Node *parent, const float resCap)
{
#ifdef STATS
	++ numPushes;
#endif

	if (resCap >= child->excess)
	{
		parent->excess += child->excess;
		currentArc->flow += child->excess;
		child->excess = 0;
		return;
	}

	currentArc->direction = 0;
	parent->excess += resCap;
	child->excess -= resCap;
	currentArc->flow = currentArc->capacity;
	parent->outOfTree[parent->numOutOfTree] = currentArc;
	++ parent->numOutOfTree;
	breakRelationship (parent, child);

	addToStrongBucket (child, strongRoots[child->label].end);
}


static inline void
pushDownward (Arc *currentArc, Node *child, Node *parent, float flow)
{
#ifdef STATS
	++ numPushes;
#endif

	if (flow >= child->excess)
	{
		parent->excess += child->excess;
		currentArc->flow -= child->excess;
		child->excess = 0;
		return;
	}

	currentArc->direction = 1;
	child->excess -= flow;
	parent->excess += flow;
	currentArc->flow = 0;
	parent->outOfTree[parent->numOutOfTree] = currentArc;
	++ parent->numOutOfTree;
	breakRelationship (parent, child);

	addToStrongBucket (child, strongRoots[child->label].end);
}

static void
pushExcess (Node *strongRoot)
{
	Node *current, *parent;
	Arc *arcToParent;

	for (current = strongRoot; (current->excess && current->parent); current = parent)
	{
		parent = current->parent;
		arcToParent = current->arcToParent;
		if (arcToParent->direction)
		{
			pushUpward (arcToParent, current, parent, (arcToParent->capacity - arcToParent->flow));
		}
		else
		{
			pushDownward (arcToParent, current, parent, arcToParent->flow);
		}
	}

	if (current->excess > 0)
	{
		if (!current->next)
		{
			addToStrongBucket (current, strongRoots[current->label].end);
		}
	}
}


static Arc *
findWeakNode (Node *strongNode, Node **weakNode)
{
	int i, size;
	Arc *out;

	size = strongNode->numOutOfTree;

	for (i=strongNode->nextArc; i<size; ++i)
	{

#ifdef STATS
		++ numArcScans;
#endif

		if (strongNode->outOfTree[i]->to->label == (highestStrongLabel-1))
		{
			strongNode->nextArc = i;
			out = strongNode->outOfTree[i];
			(*weakNode) = out->to;
			-- strongNode->numOutOfTree;
			strongNode->outOfTree[i] = strongNode->outOfTree[strongNode->numOutOfTree];
			return (out);
		}
		else if (strongNode->outOfTree[i]->from->label == (highestStrongLabel-1))
		{
			strongNode->nextArc = i;
			out = strongNode->outOfTree[i];
			(*weakNode) = out->from;
			-- strongNode->numOutOfTree;
			strongNode->outOfTree[i] = strongNode->outOfTree[strongNode->numOutOfTree];
			return (out);
		}
	}

	strongNode->nextArc = strongNode->numOutOfTree;

	return NULL;
}


static void
checkChildren (Node *curNode)
{
	for ( ; (curNode->nextScan); curNode->nextScan = curNode->nextScan->next)
	{
		if (curNode->nextScan->label == curNode->label)
		{
			return;
		}

	}

	-- labelCount[curNode->label];
	++	curNode->label;
	++ labelCount[curNode->label];

#ifdef STATS
	++ numRelabels;
#endif

	curNode->nextArc = 0;
}

static void
processRoot (Node *strongRoot)
{
	Node *temp, *strongNode = strongRoot, *weakNode;
	Arc *out;

	strongRoot->nextScan = strongRoot->childList;

	if ((out = findWeakNode (strongRoot, &weakNode)))
	{
		merge (weakNode, strongNode, out);
		pushExcess (strongRoot);
		return;
	}

	checkChildren (strongRoot);

	while (strongNode)
	{
		while (strongNode->nextScan)
		{
			temp = strongNode->nextScan;
			strongNode->nextScan = strongNode->nextScan->next;
			strongNode = temp;
			strongNode->nextScan = strongNode->childList;

			if ((out = findWeakNode (strongNode, &weakNode)))
			{
				merge (weakNode, strongNode, out);
				pushExcess (strongRoot);
				return;
			}

			checkChildren (strongNode);
		}

		if ((strongNode = strongNode->parent))
		{
			checkChildren (strongNode);
		}
	}

	addToStrongBucket (strongRoot, strongRoots[strongRoot->label].end);

	++ highestStrongLabel;
}

static Node *
getHighestStrongRoot (const int theparam)
{	
	int i;
	Node *strongRoot;

	for (i=highestStrongLabel; i>0; --i)
	{
		if (strongRoots[i].start->next != strongRoots[i].end)
		{
			highestStrongLabel = i;
			if (labelCount[i-1])
			{
				strongRoot = strongRoots[i].start->next;
				strongRoot->next->prev = strongRoot->prev;
				strongRoot->prev->next = strongRoot->next;
				strongRoot->next = NULL;
				return strongRoot;
			}

			while (strongRoots[i].start->next != strongRoots[i].end)
			{

#ifdef STATS
				++ numGaps;
#endif
				strongRoot = strongRoots[i].start->next;
				strongRoot->next->prev = strongRoot->prev;
				strongRoot->prev->next = strongRoot->next;
				liftAll (strongRoot, theparam);
			}
		}
	}

	if (strongRoots[0].start->next == strongRoots[0].end)
	{
		return NULL;
	}

	while (strongRoots[0].start->next != strongRoots[0].end)
	{
		strongRoot = strongRoots[0].start->next;
		strongRoot->next->prev = strongRoot->prev;
		strongRoot->prev->next = strongRoot->next;

		strongRoot->label = 1;
		-- labelCount[0];
		++ labelCount[1];

#ifdef STATS
		++ numRelabels;
#endif

		addToStrongBucket (strongRoot, strongRoots[strongRoot->label].end);
	}

	highestStrongLabel = 1;

	strongRoot = strongRoots[1].start->next;
	strongRoot->next->prev = strongRoot->prev;
	strongRoot->prev->next = strongRoot->next;
	strongRoot->next = NULL;

	return strongRoot;
}

static void
updateCapacities (const int theparam)
{
	int i, size;
	float delta;
	Arc *tempArc;
	Node *tempNode;

	size = adjacencyList[source-1].numOutOfTree;
	for (i=0; i<size; ++i)
	{
		tempArc = adjacencyList[source-1].outOfTree[i];
		delta = (tempArc->capacities[theparam] - tempArc->capacity);
		if (delta < 0)
		{
			printf ("c Error on source-adjacent arc (%d, %d): capacity decreases by %f at parameter %d.\n",
				tempArc->from->number,
				tempArc->to->number,
				(-delta),
				(theparam+1));
			exit(0);
		}

		tempArc->capacity += delta;
		tempArc->flow += delta;
		tempArc->to->excess += delta;

		if ((tempArc->to->label < numNodes) && (tempArc->to->excess > 0))
		{
			pushExcess (tempArc->to);
		}
	}

	size = adjacencyList[sink-1].numOutOfTree;
	for (i=0; i<size; ++i)
	{
		tempArc = adjacencyList[sink-1].outOfTree[i];
		delta = (tempArc->capacities[theparam] - tempArc->capacity);
		if (delta > 0)
		{
			printf ("c Error on sink-adjacent arc (%d, %d): capacity %f increases to %f at parameter %d.\n",
				tempArc->from->number,
				tempArc->to->number,
				tempArc->capacity,
				tempArc->capacities[theparam],
				(theparam+1));
			exit(0);
		}

		tempArc->capacity += delta;
		tempArc->flow += delta;
		tempArc->from->excess -= delta;

		if ((tempArc->from->label < numNodes) && (tempArc->from->excess > 0))
		{
			pushExcess (tempArc->from);
		}
	}

	highestStrongLabel = (numNodes-1);
}

static void
pseudoflowPhase1 (void)
{
	Node *strongRoot;
	int theparam = 0;
	double thetime;

	while ((strongRoot = getHighestStrongRoot (theparam)))
	{
		processRoot (strongRoot);
	}

	for (theparam=1; theparam < numParams; ++ theparam)
	{
		updateCapacities (theparam);
		while ((strongRoot = getHighestStrongRoot (theparam)))
		{
			processRoot (strongRoot);
		}
	}
}
