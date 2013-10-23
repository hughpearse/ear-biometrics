import weka.core.DistanceFunction;
import weka.core.EuclideanDistance;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;
import weka.core.neighboursearch.KDTree;

public class Search {


	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length < 2) {
			System.out.println("You must enter 2 arguments.");
			System.out.println("java -jar search.jar 1,2,3,4,5,6,7,8,9,10 /path/to/arff/file");
			System.exit(0);
		}
		
		try {
			String file = args[1].toString();
			Points instanceCreator = new Points();
			DataSource source = new DataSource(file);
			Instances data = source.getDataSet();
			// setting class attribute if the data format does not provide this information
			// For example, the XRFF format saves the class attribute information as well
			if (data.classIndex() == -1)
				data.setClassIndex(data.numAttributes() - 1);
			
			//---------------------------------------------
			// Create the weka datastructure for 3D Points
			//---------------------------------------------
			//Instances wekaPoints1 = insertIntoWeka(points1, "wekaPoints1");
			
			// Set up the KDTree
			KDTree tree = new KDTree();    
			try {
			    tree.setInstances(data);
			     
			    EuclideanDistance df = new EuclideanDistance(data);
			    df.setDontNormalize(true);
			     
			    tree.setDistanceFunction(df);          
			}
			catch (Exception e) { e.printStackTrace();}
			
			//---------------------------------------------
			// let's search for the nearest and second nearest neighbor
			//---------------------------------------------
			//56,73,104,137,168,139,152,192,177,197
			String myargs[] = args[0].toString().split(",");
			Integer searchQuery[] = {0,0,0,0,0,0,0,0,0,0};
			for(int i = 0; i<myargs.length; i++){
				searchQuery[i] = Integer.parseInt(myargs[i]);
			}
			
			Instance nn1, nn2;
			final Instance p = instanceCreator.createInstance(searchQuery, data);
			try
			{
			    Instances neighbors = tree.kNearestNeighbours(p, 2);
			    nn1 = neighbors.instance(0);
			    nn2 = neighbors.instance(1);                           
			} catch (Exception e) {
				nn1 = nn2 = null;
			}
			
			String[] parts = nn1.toString().split(",");
			System.out.println(parts[10]);
			//System.out.println(nn1 + " is the nearest neigbour for " + p);
			//System.out.println(nn2 + " is the second nearest neigbour for " + p);
			 
			// Now we can also easily compute the distances as the KDTree does it
			DistanceFunction df = tree.getDistanceFunction();
			//System.out.println("The distance between" + nn1 + " and " + p + " is " + df.distance(nn1, p));
			//System.out.println("The distance between" + nn2 + " and " + p + " is " + df.distance(nn2, p));
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}//end main

}//end class
