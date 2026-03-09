import sys
import numpy as np
from sklearn.metrics import silhouette_score
import symnmf
import kmeans

def main():
    if len(sys.argv) != 3:
        return
    try:
        k = int(sys.argv[1])
    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1)
    file_name = sys.argv[2]
   
    #using the read_points function from symnmf to read the data points
    points = symnmf.read_points(file_name)
    #calculate the silhouette score for kmeans
    kmeans_score = kmeans_score_func(points, k)
    #calculate the silhouette score for nmf
    H = symnmf.main_function(points, k,"symnmf")
    nmf_labels = np.argmax(H, axis=1)
    nmf_score = silhouette_score(points, nmf_labels)
    
    #print the scores
    print(f"nmf: {nmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")

#calculate silhouette score from kmeans results (HW1)
def kmeans_score_func(points, k):
    if(k<1 or k>=len(points)):
        print("An Error Has Occurred")
        sys.exit(1)
    
    centroids = kmeans.kmeans(k,0.0001,400, points)
    centroids = np.array(centroids)
    kmeans_labels = []
    #assign cluster to each point according to kmeans centroids results
    for point in points:
        distances = np.linalg.norm(centroids - point, axis=1)
        kmeans_labels.append(np.argmin(distances))
    points = np.array(points)
    return silhouette_score(points, kmeans_labels)
    

if __name__ == "__main__":
    np.random.seed(1234)
    main()