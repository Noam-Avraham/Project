import sys
import numpy as np
import symnmf


def main():
    np.random.seed(1234)
    #check for correct number of arguments and validity of arguments
    if len(sys.argv) != 4:
		print("An Error Has Occurred")
		sys.exit(1)
	try:
		k = int(sys.argv[1])
	except ValueError:
		print("Incorrect number of clusters!")
		sys.exit(1)
    if(k <= 1):
        print("Incorrect number of clusters!")
        sys.exit(1)
	
    goal = (sys.argv[2])
    if goal != "symnmf" and goal != "sym" and goal != "ddg" and goal != "norm":
        print("Invalid goal!")
        sys.exit(1)
	
    file_name = sys.argv[3]
    points = read_points()

	if not points:
		print("An Error Has Occurred")
		sys.exit(1)
	
    n = len(points)
    d= len(points[0])

    if k <= 1 or k >= n:
        print("Incorrect number of clusters!")
        sys.exit(1)
    #if goal isnt symnmf,send information to C
    if(goal == "sym"):
        #senfing to C sym()
        final_matrix=sym(points, k,n,d, goal)
   
    elif(goal == "ddg"):
        #senfing to C ddg()
        final_matrix=ddg(points, k,n,d, goal)
    
    elif(goal == "norm"):
        #sending to C norm()
        final_matrix=norm(points, k,n,d, goal)
   
    elif(goal == "symnmf"):
        A=similarity_matrix(points)
        
        D=diagonal_degree_matrix(A)
       
        W=weight_matrix(D,A)
        
        start_H=initial_H(W,n,k)
        #sending to C symnmf()
        final_matrix=symnmf(W,start_H,n,k,goal)
    
    
    #print matrix according to the format
    print_matrix(final_matrix)   
    sys.exit(0)

def similarity_matrix(points):
    X=np.array(points)
    #in norms i,j (a_i)^2 +(a_j)^2
    norms = np.sum(X**2, axis=1).reshape(-1, 1)
    #a_i,j is the distance a_i and a_j,  ||a_i - a_j||^2 = ||a_i||^2 + ||a_j||^2 - 2 * a_i . a_j
    distances = norms + norms.T - 2 * np.dot(X, X.T)

    distances = np.maximum(distances, 0)

    A = np.exp(-distances / 2)
    np.fill_diagonal(A, 0)
    
    return A

def diagonal_degree_matrix(A):
    D = np.diag(np.sum(A, axis=1))
    return D

def weight_matrix(D,A):
    D_inv_sqrt = np.power(D, -0.5)
    #handling inf values after division by zero
    D_inv_sqrt[np.isinf(D_inv_sqrt)] = 0
    W = D_inv_sqrt @ A @ D_inv_sqrt
    return W
    
   
   
def print_matrix(matrix):
    for row in matrix:
        st=[]
        for x in range(len(row)):
            st.append('{0:.4f}'.format(row[x]))
        print(','.join(st))

def initial_H(W,n,k):
    W_np = np.array(W)
    m = np.mean(W_np)
    
    upper_bound = 2 * np.sqrt(m / k)
    
    H = np.random.uniform(0, upper_bound, size=(n, k))

    return H.tolist()

def euclidean_distance(x, y):
    point1 = np.array(x)
    point2 = np.array(y)
    return np.sqrt(np.sum((point1 - point2) ** 2))

#read all data points.
def read_points():
	#pts is list of list of floats
	points = []
	all_input = sys.stdin.read().strip()
	lines=all_input.splitlines()

	for line in lines:
		if not line.strip():
			continue
		coards_string=line.split(',')
		current_point=[]
		
		for coard in coards_string:
			try:
				coard_float = float(coard)
			except ValueError:
				print("An Error Has Occurred")
				sys.exit(1)
			coard_float=float(coard)
			current_point.append(coard_float)
		points.append(current_point)

	return points
	


if __name__ == "__main__":
    main()