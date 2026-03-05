import sys
import numpy as np
from sklearn.metrics import silhouette_score
import symnmf
import kmeans

def main():
    if len(sys.argv) < 3:
        return
    
    k = int(sys.argv[1])
    file_name = sys.argv[2]
    #using the read_points function from symnmf to read the data points
    points = read_points(file_name)

    H = symnmf.main_function(points, k,"symnmf")
    nmf_labels = np.argmax(H, axis=1)
    nmf_score = silhouette_score(points, nmf_labels)

    centroids = kmeans.kmeans(k,0.0001,400, points)
    centroids = np.array(centroids)
    
    kmeans_labels = []
    for point in points:
        distances = np.linalg.norm(centroids - point, axis=1)
        kmeans_labels.append(np.argmin(distances))
    
    kmeans_score = silhouette_score(points, kmeans_labels)
    
    # הדפסה בפורמט הנדרש
    print(f"nmf: {nmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")
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
    main()