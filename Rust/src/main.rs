extern crate rand;

use rand::Rng;
use rayon::prelude::*;
use std::{
    io::{self},
    time::Instant,
};

fn main() {
    let size: usize;
    let iters: i32;
    let mut input = String::new();

    println!("Matrix size: ");
    io::stdin().read_line(&mut input).expect("Invalid input");
    size = input.trim().parse::<usize>().unwrap();

    input = String::new();

    println!("Iterations: ");
    io::stdin().read_line(&mut input).expect("Invalid input");
    iters = input.trim().parse::<i32>().unwrap();

    println!("Threads: {}\n", rayon::current_num_threads());

    let mut m1: Vec<Vec<i32>> = vec![vec![0; size]; size];
    let mut m2: Vec<Vec<i32>> = vec![vec![0; size]; size];

    let mut total_clean: i32 = 0;
    let mut total_rayon: i32 = 0;

    for _ in 0..iters {
        random_matrix(&mut m1);
        random_matrix(&mut m2);

        let mut start: std::time::Instant;
        let mut duration: std::time::Duration;


        start = Instant::now();
        clean(&m1, &m2);
        duration = start.elapsed();
        println!("Clean: {:?}", duration.as_millis());
        total_clean += i32::try_from(duration.as_millis()).unwrap();

        start = Instant::now();
        with_rayon(&m1, &m2);
        duration = start.elapsed();
        println!("Rayon: {:?}\n", duration.as_millis());
        total_rayon += i32::try_from(duration.as_millis()).unwrap();
    }

    println!("Avg clean: {} ms", f64::from(total_clean) / f64::from(iters));
    println!("Avg rayon: {} ms", f64::from(total_rayon) / f64::from(iters));
    println!("Diff clean / rayon: {:.2}%", (f64::from(total_clean) / f64::from(total_rayon) - 1.0) * 100.0);
}

fn with_rayon(m1: &Vec<Vec<i32>>, m2: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    m1.par_iter()
        .zip(m2.par_iter())
        .map(|(row_m1, row_m2)| {
            row_m1
                .par_iter()
                .zip(row_m2.par_iter())
                .map(|(x, y)| x * y)
                .collect()
        })
        .collect()
}

fn clean(m1: &Vec<Vec<i32>>, m2: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    m1.iter()
        .zip(m2.iter())
        .map(|(row_m1, row_m2)| {
            row_m1
                .iter()
                .zip(row_m2.iter())
                .map(|(x, y)| x * y)
                .collect()
        })
        .collect()
}

fn random_matrix(m: &mut Vec<Vec<i32>>) {
    let size: usize = m.len();
    let mut random = rand::thread_rng();

    for i in 0..size {
        for j in 0..size {
            m[i][j] = random.gen_range(0..32767);
        }
    }
}
