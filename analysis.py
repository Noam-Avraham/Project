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
    points = symnmf.read_points(file_name)

    H = symnmf.main_function(points, k,"symnmf")
    nmf_labels = np.argmax(H, axis=1)
    nmf_score = silhouette_score(points, nmf_labels)

    centroids = kmeans.kmeans(k,0.0001,400, points)

    kmeans_labels = []
    for point in points:
        distances = np.linalg.norm(centroids - point, axis=1)
        kmeans_labels.append(np.argmin(distances))
    
    kmeans_score = silhouette_score(points, kmeans_labels)
    
    # הדפסה בפורמט הנדרש
    print(f"nmf: {nmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")

if __name__ == "__main__":
    main()