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
    
    if k <= 1 or k >= n:
        print("Incorrect number of clusters!")
        sys.exit(1)
    #if goal isnt symnmf,send information to C
    if(goal != "symnmf"):
        final_matrix=symnmf.fitx(points, k,n, goal)
    
    #if goal is symnmf, preform full symnmf algorithm (except H calculation) in C
    if(goal == "symnmf"):
        A=similarity_matrix(points,k,n)
       
        D=diagonal_degree_matrix(A,n)
       
        W=weight_matrix(D,A,n)
        
        start_H=initial_H(W,n,k)
        #sending W,start_H,n,k,goal to C and getting final_H from C
        final_matrix=symnmf.fitH(W,start_H,n,k,goal)
    #print matrix according to the format
    print_matrix(final_matrix)   
    sys.exit(0)
    
    
   
   
def print_matrix(matrix):
    for row in matrix:
        st=[]
		for x in range(len(row)):
			st.append('{0:.4f}'.format(row[x]))
		print(','.join(st))

def initial_H(W,n,k):
W_np = np.array(W)
    W_np = W_np.shape[0]
    m = np.mean(W_np)
    
    upper_bound = 2 * np.sqrt(m / k)
    
    H = np.random.uniform(0, upper_bound, size=(n, k))

    return H.tolist()


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