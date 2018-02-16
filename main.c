/*
    Hamiltonian is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Hamiltonian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Hamiltonian.  If not, see <http://www.gnu.org/licenses/>.
*/


/*AUTHOR: SAXON SUPPLE*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>



int *squares;

typedef struct _node
{
	int num;
	struct _node *next;
} NODE;

void generate_squares(int count)
{
	squares = (int*)malloc(sizeof(int) * count);
	for (int i = 1; i <= count; ++i)
		squares[i-1] = i * i;
}

void free_squares(void)
{
	free(squares);
}

char is_square(int square)
{
	for (int i = 0; i < 15; ++i)
	{
		if (squares[i] == square)
			return 1;
	}
	return 0;
}

void fill_node(NODE **graph, int num, int count)
{
        NODE *p = NULL;

	for (int i = 1; i <= count; ++i)
	{
		if (i == num)
			continue;
		if (is_square(num + i))
		{
			if (p == NULL)
			{
				p = (NODE*)malloc(sizeof(NODE));
				graph[num-1] = p;
			}
			else
			{
				p->next = (NODE*)malloc(sizeof(NODE));
				p = p->next;	
			}
			p->num = i;
		}
	}
	if (p != NULL)
		p->next = NULL;
}

NODE **init_graph(int count)
{
	NODE **graph = (NODE**)malloc(sizeof(NODE*) * count);
	for (int i = 1; i <= count; ++i)
	{
		fill_node(graph, i, count);
	}
	return graph;
}

void free_graph(NODE **graph, int count)
{
        NODE *p = *graph;
	int i = 0;
	while(i < count)
	{
		NODE *tmp = p;
		NODE *ptr = p;
		while (ptr != NULL)
		{
			ptr = ptr->next;
			free(tmp);
			tmp = ptr;
		}
		++p;
		++i;
	}
	free(graph);
}

int *solution;

char *nodes;

static inline char full(int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (nodes[i] == 0)
			return 0;
	}
	return 1;
}

int solve(NODE **graph, int node, int pos, int count)
{
	NODE *p = graph[node-1];
	nodes[node-1] = 1;

	solution[pos] = node;
	while (p != NULL)
	{
		if (nodes[p->num-1] == 0)
		{
			if (full(count))
			{
				solution[pos] = node;
				return 1;
			}
			if (solve(graph, p->num, pos+1, count) == 1)
			{
				solution[pos] = node;
				return 1;
			}
		}
		p = p->next;
	}
	if (full(count))
	{
		solution[pos] = node;
		return 1;
	}
	nodes[node-1] = 0;

	return -1;
}


int main(int argc, char **argv)
{
	if (argc < 2)
		exit(-1);
	int count = atoi(argv[1]);

	const rlim_t k_stack_size = 64L * 1024L * 1024L * 300L;
	struct rlimit rl;
	int result;

	result = getrlimit(RLIMIT_STACK, &rl);
	if (result == 0)
	{
		if (rl.rlim_cur < k_stack_size)
		{
			rl.rlim_cur = k_stack_size;
			result = setrlimit(RLIMIT_STACK, &rl);
			if (result != 0)
			{
				fprintf(stderr, "setrlimit returned result = %d\n", result);
			}
		}
	}
	
	generate_squares(count);
	
	NODE **graph = init_graph(count);
	//free_graph(graph, count);

	solution = (int*)malloc(count * sizeof(int));

	nodes = malloc(count);
	memset(nodes, 0, count);

	int ret;
	for (int i = 1; i <= count; ++i)
	{
	        memset(nodes, 0, count);
		ret = solve(graph, i, 0, count);
		if (ret == 1)
		{
			for (int i = 0; i < count; ++i)
			{
				printf("%d ", solution[i]);
			}
			printf("\n");
		}
	}

	free(solution);
	free(nodes);
	free_squares();

	return 0;
}
