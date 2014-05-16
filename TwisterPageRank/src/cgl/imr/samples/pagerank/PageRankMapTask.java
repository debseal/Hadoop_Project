/*
 * Software License, Version 1.0
 *
 *  Copyright 2003 The Trustees of Indiana University.  All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1) All redistributions of source code must retain the above copyright notice,
 *  the list of authors in the original source code, this list of conditions and
 *  the disclaimer listed in this license;
 * 2) All redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the disclaimer listed in this license in
 *  the documentation and/or other materials provided with the distribution;
 * 3) Any documentation included with all redistributions must include the
 *  following acknowledgement:
 *
 * "This product includes software developed by the Community Grids Lab. For
 *  further information contact the Community Grids Lab at
 *  http://communitygrids.iu.edu/."
 *
 *  Alternatively, this acknowledgement may appear in the software itself, and
 *  wherever such third-party acknowledgments normally appear.
 *
 * 4) The name Indiana University or Community Grids Lab or Twister,
 *  shall not be used to endorse or promote products derived from this software
 *  without prior written permission from Indiana University.  For written
 *  permission, please contact the Advanced Research and Technology Institute
 *  ("ARTI") at 351 West 10th Street, Indianapolis, Indiana 46202.
 * 5) Products derived from this software may not be called Twister,
 *  nor may Indiana University or Community Grids Lab or Twister appear
 *  in their name, without prior written permission of ARTI.
 *
 *
 *  Indiana University provides no reassurances that the source code provided
 *  does not infringe the patent or any other intellectual property rights of
 *  any other entity.  Indiana University disclaims any liability to any
 *  recipient for claims brought by any other entity based on infringement of
 *  intellectual property rights or otherwise.
 *
 * LICENSEE UNDERSTANDS THAT SOFTWARE IS PROVIDED "AS IS" FOR WHICH NO
 * WARRANTIES AS TO CAPABILITIES OR ACCURACY ARE MADE. INDIANA UNIVERSITY GIVES
 * NO WARRANTIES AND MAKES NO REPRESENTATION THAT SOFTWARE IS FREE OF
 * INFRINGEMENT OF THIRD PARTY PATENT, COPYRIGHT, OR OTHER PROPRIETARY RIGHTS.
 * INDIANA UNIVERSITY MAKES NO WARRANTIES THAT SOFTWARE IS FREE FROM "BUGS",
 * "VIRUSES", "TROJAN HORSES", "TRAP DOORS", "WORMS", OR OTHER HARMFUL CODE.
 * LICENSEE ASSUMES THE ENTIRE RISK AS TO THE PERFORMANCE OF SOFTWARE AND/OR
 * ASSOCIATED MATERIALS, AND TO THE PERFORMANCE AND VALIDITY OF INFORMATION
 * GENERATED USING SOFTWARE.
 */

package cgl.imr.samples.pagerank;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

import cgl.imr.base.Key;
import cgl.imr.base.MapOutputCollector;
import cgl.imr.base.MapTask;
import cgl.imr.base.SerializationException;
import cgl.imr.base.TwisterException;
import cgl.imr.base.Value;
import cgl.imr.base.impl.JobConf;
import cgl.imr.base.impl.MapperConf;
import cgl.imr.data.file.FileData;
import cgl.imr.types.BytesValue;
import cgl.imr.types.DoubleVectorData;
import cgl.imr.types.IntKey;

/*
 * Map task for the page rank.
 *
 * @author Hui Li (lihui@indiana.edu) 
 * @author James Fox (foxjas09@gmail.com)
 */

public class PageRankMapTask implements MapTask {

  // data structure of one item of adjacency matrix
  public class UrlData {
    public int index; // "from" index
    public ArrayList<Integer> urls;
  }

  private FileData fileData;
  private int numUrls; // number of urls of all tasks
  private int numUrlsInTask; // number of urls of current map task
  private UrlData[] UrlsData; // the adjacency matrix of all urls

  public void close() throws TwisterException {
  }

  /*
   * Used to load the vector data from files. Since the mappers are cached
   * across iterations, we only need to load this data from file once for all
   * the iterations.
   */

  public void configure(JobConf jobConf, MapperConf mapConf)
    throws TwisterException {
    fileData = (FileData) mapConf.getDataPartition();
    try {
      loadDataFromFile(fileData.getFileName());
    } catch (Exception e) {
      throw new TwisterException(e);
    }
  }

  /*
   * Map task in page rank algorithm
   * 
   * @parameter collector -used to store and emit the intermediate key value
   * pair.
   * 
   * @parameter key -the index of map task.
   * 
   * @parameter val -the compressed changed page rank values.
   */

  // construct the adjacency matrix of the partitioned data
  public void loadDataFromFile(String fileName) throws IOException {
    File file = new File(fileName);
    BufferedReader reader = new BufferedReader(new FileReader(file));
    String inputLine = reader.readLine();
    this.numUrlsInTask = Integer.parseInt(inputLine); // num of urls that
    // current map task
    // have
    UrlsData = new UrlData[numUrlsInTask];
    String[] vectorValues;

    for (int i = 0; i < numUrlsInTask; i++) {
      UrlsData[i] = new UrlData();
      UrlsData[i].urls = new ArrayList<Integer>();
      inputLine = reader.readLine();
      vectorValues = inputLine.split(" ");
      UrlsData[i].index = Integer.parseInt(vectorValues[0]);
      for (int j = 1; j < vectorValues.length; j++) {
        UrlsData[i].urls.add(Integer.valueOf(vectorValues[j]));
      }// end for j
    }// end for i
    reader.close();
  }// end loadDataFromFile

  public void map(MapOutputCollector collector, Key key, Value val)
    throws TwisterException {
    try {
      DoubleVectorData tmpDV = new DoubleVectorData();
      tmpDV.fromBytes(val.getBytes());
      double[][] tmpPageRanks = tmpDV.getData();
      this.numUrls = tmpPageRanks.length;
      double[][] newPageRanks = new double[numUrls][1]; // last entry reserved
                                                        // for storing
                                                        // danglingValSum for
                                                        // this partition

      
      double danglingValSum = 0.0d;
      int fromUrl = -1;
      int toUrl = -1;
      
      /** Your solution here */
      
      // first calculate new pagerank value based on the fromUrl and toUrl.
        
      // add the factor in dangling values to all pagerank values

      // include damping factor (scaled to this partition)


      /** End of your solution */
      DoubleVectorData resultDV = new DoubleVectorData(newPageRanks,
        this.numUrls, 1);
      int taskNo = key.hashCode();
      // System.out.println("MAP Task ID: " + taskNo);
      collector
        .collect(new IntKey(taskNo), new BytesValue(resultDV.getBytes()));

    } catch (SerializationException e) {
      throw new TwisterException(e);
    }
  }// end map
}
