import sys
import numpy as np
import symnmf_module as symnmf


def main():
    np.random.seed(1234)
    #check for correct number of arguments and validity of arguments
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        sys.exit(1)
    try:
        k = int(sys.argv[1])
    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1)
    if(k <= 1):
        print("An Error Has Occurred")
        sys.exit(1)

    goal = (sys.argv[2])
    if goal != "symnmf" and goal != "sym" and goal != "ddg" and goal != "norm":
        print("An Error Has Occurred")
        sys.exit(1)
    #assuming file name valid
    file_name = sys.argv[3]
    points = read_points(file_name)   
    final_matrix = main_function(points, k, goal)
   
    #print matrix according to the format
    print_matrix(final_matrix)   
    sys.exit(0)

# diffrent function that can be used by analysis.py to get the H matrix.
def main_function(points, k, goal):
    if not points:
        print("An Error Has Occurred")
        sys.exit(1)
    
    n = len(points)
    d = len(points[0])

    if k <= 1 or k >= n:
        print("An Error Has Occurred")
        sys.exit(1)
    #if goal is not symnmf,send information to C
    if(goal == "sym"):
        #senfing to C sym()
        final_matrix=symnmf.sym(points, d, n)
   
    elif(goal == "ddg"):
        #senfing to C ddg()
        final_matrix=symnmf.ddg(points, d, n)
    
    elif(goal == "norm"):
        #sending to C norm()
        final_matrix=symnmf.norm(points, d, n)
   
    elif(goal == "symnmf"):
        W=symnmf.norm(points, d, n)
        
        start_H=initial_H(W,n,k)
        #sending to C symnmf()
        final_matrix=symnmf.symnmf(W,start_H,n,k)

    return final_matrix
    
#function to print the matrix acccording to the format.
def print_matrix(matrix):
    for row in matrix:
        st=[]
        for x in range(len(row)):
            st.append('{0:.4f}'.format(row[x]))
        print(','.join(st))
#calculate the initial H matrix.
def initial_H(W,n,k):
    W_np = np.array(W)
    m = np.mean(W_np)
    upper_bound = 2 * np.sqrt(m / k)
    
    H = np.random.uniform(0, upper_bound, size=(n, k))
    
    return H.tolist()


#read all data points.
def read_points(file_name):
    # pts is list of list of floats
    points = []
    try:
        with open(file_name, 'r') as f:
            lines = f.read().strip().splitlines()
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

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
    np.random.seed(1234)
    main()